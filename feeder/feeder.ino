#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiManager.h>
#include <UniversalTelegramBot.h>
#include <Preferences.h>
#include "soc/rtc.h"

// Constants
const char* DEFAULT_AP_NAME = "PetFeeder";
const char* DEFAULT_AP_PASSWORD = "11111111";
const char* PREF_TELEGRAM = "telegram";
const char* PREF_TELEGRAM_BOT_TOKEN_KEY = "botToken";
const char* PREF_TELEGRAM_GROUP_ID_KEY = "groupId";
const char* PREF_TELEGRAM_LAST_MESSAGE_ID = "lastMessageId";
const char* DEFAULT_TELEGRAM_BOT_TOKEN = "";
const char* DEFAULT_TELEGRAM_GROUP_ID = "";
const long DEFAULT_TELEGRAM_MESSAGE_ID = 0L;
const char* TELEGRAM_CERT = TELEGRAM_CERTIFICATE_ROOT;
const int CONFIG_PORTAL_TIMEOUT_S = 300;
const int TELEGRAM_MAX_RETRIES = 3;
const int TELEGRAM_RETRY_DELAY_MS = 2000;
const uint64_t DEEP_SLEEP_DURATION_US = 60000000;  // 1 minute in microseconds

const char* WELCOME_MESSAGE = "Устройство готово к использованию.\n\n"
                              "Доступные комманды:\n\n"
                              "1) /schedule 9:00,13:00,19:00 - установка графика кормления.";

// Persistent variables
RTC_DATA_ATTR bool initialSetupDone = true;
RTC_DATA_ATTR bool firstLoop = true;

// Global objects
Preferences preferences;
WiFiClientSecure securedClient;

// Setup function
void setup() {
  WiFi.mode(WIFI_STA);
  Serial.begin(115200);

  securedClient.setCACert(TELEGRAM_CERT);

  if (!initialSetupDone) {
    WiFiManager wm;

    wm.setConfigPortalTimeout(CONFIG_PORTAL_TIMEOUT_S);

    preferences.begin(PREF_TELEGRAM, false);

    String botToken = preferences.getString(PREF_TELEGRAM_BOT_TOKEN_KEY, DEFAULT_TELEGRAM_BOT_TOKEN);
    String groupId = preferences.getString(PREF_TELEGRAM_GROUP_ID_KEY, DEFAULT_TELEGRAM_GROUP_ID);

    WiFiManagerParameter custom_bot_token(PREF_TELEGRAM_BOT_TOKEN_KEY, "Telegram Bot Token", botToken.c_str(), 64);
    WiFiManagerParameter custom_group_id(PREF_TELEGRAM_GROUP_ID_KEY, "Telegram Group ID", groupId.c_str(), 20);

    wm.addParameter(&custom_bot_token);
    wm.addParameter(&custom_group_id);

    wm.startConfigPortal(DEFAULT_AP_NAME, DEFAULT_AP_PASSWORD);

    botToken = custom_bot_token.getValue();
    groupId = custom_group_id.getValue();

    preferences.putString(PREF_TELEGRAM_BOT_TOKEN_KEY, botToken);
    preferences.putString(PREF_TELEGRAM_GROUP_ID_KEY, groupId);

    preferences.end();

    initialSetupDone = true;
  }
}

// Send message to Telegram with retry
void sendBotMessage(const String botToken, const String chatId, const String message) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Telegram bot: sendBotMessage - Wi-Fi not connected!");
    return;
  }

  if (botToken.isEmpty()) {
    Serial.println("Telegram bot: sendBotMessage - bot token not provided!");
    return;
  }

  if (chatId.isEmpty()) {
    Serial.println("Telegram bot: sendBotMessage - chatId not provided!");
    return;
  }

  UniversalTelegramBot bot(botToken, securedClient);

  int retryCount = 0;
  bool success = false;

  while (retryCount < TELEGRAM_MAX_RETRIES && !success) {
    success = bot.sendMessage(chatId, message);

    if (!success) {
      Serial.println("Telegram bot: sendBotMessage - error sending message. Retrying...");
      retryCount++;
      delay(TELEGRAM_RETRY_DELAY_MS);
    }
  }

  if (!success) {
    Serial.println("Telegram bot: sendBotMessage - failed to send message after retries!");
  }
}

void saveLastBotMessageReceived(long lastMessageId) {
  Serial.println("Telegram bot: saveLastBotMessageReceived - Start saving last message ID: " + String(lastMessageId));

  preferences.begin(PREF_TELEGRAM, false);

  Serial.println("Telegram bot: saveLastBotMessageReceived - Preferences opened");

  preferences.putLong(PREF_TELEGRAM_LAST_MESSAGE_ID, lastMessageId);

  Serial.println("Telegram bot: saveLastBotMessageReceived - Last message ID saved");

  preferences.end();

  Serial.println("Telegram bot: saveLastBotMessageReceived - Preferences closed");
}



void retryLastBotMessageReceived(UniversalTelegramBot& bot) {
  Serial.println("Telegram bot: retryLastBotMessageReceived - Start getting last message ID");

  preferences.begin(PREF_TELEGRAM, false);

  Serial.println("Telegram bot: retryLastBotMessageReceived - Preferences opened");

  long lastMessageId = preferences.getLong(PREF_TELEGRAM_LAST_MESSAGE_ID, DEFAULT_TELEGRAM_MESSAGE_ID);

  Serial.println("Telegram bot: retryLastBotMessageReceived - Last message ID: " + String(lastMessageId));

  preferences.end();

  Serial.println("Telegram bot: retryLastBotMessageReceived - Preferences closed");

  bot.last_message_received = lastMessageId;

  Serial.println("Telegram bot: retryLastBotMessageReceived - Updated bot instance");
}

void handleNewMessages(UniversalTelegramBot& bot, int numNewMessages, const String chatId) {
  Serial.println("Telegram bot: handleNewMessages - Fetched messages: " + String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    if (bot.messages[i].chat_id != chatId) {
      Serial.println("Telegram bot: handleNewMessages - Message is not from the allowed chat. Skipping...");
      continue;
    }

    String text = bot.messages[i].text;

    Serial.println("Telegram bot: handleNewMessages - Message : " + text);
  }
}

void processBotMessages(const String botToken, const String chatId) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Telegram bot: processBotMessages - Wi-Fi not connected!");
    return;
  }

  if (botToken.isEmpty()) {
    Serial.println("Telegram bot: processBotMessages - bot token not provided!");
    return;
  }

  if (chatId.isEmpty()) {
    Serial.println("Telegram bot: processBotMessages - chatId not provided!");
    return;
  }

  UniversalTelegramBot bot(botToken, securedClient);

  bool success = false;
  int retryCount = 0;
  int newMessages = 0;

  while (retryCount < TELEGRAM_MAX_RETRIES && !success) {
    retryLastBotMessageReceived(bot);

    newMessages = bot.getUpdates(bot.last_message_received + 1);

    while (newMessages) {
      success = true;

      handleNewMessages(bot, newMessages, chatId);

      newMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    if (!success) {
      Serial.println("Telegram bot: processBotMessages - Fetch bot updates: Error. Retrying...");
      retryCount++;
      delay(TELEGRAM_RETRY_DELAY_MS);
    }
  }


  if (!success) {
    return;
  }

  saveLastBotMessageReceived(bot.last_message_received);
}

// Main loop
void loop() {
  WiFiManager wm;

  wm.setConfigPortalTimeout(1);
  wm.autoConnect();

  preferences.begin(PREF_TELEGRAM, true);

  String botToken = preferences.getString(PREF_TELEGRAM_BOT_TOKEN_KEY, DEFAULT_TELEGRAM_BOT_TOKEN);
  String groupId = preferences.getString(PREF_TELEGRAM_GROUP_ID_KEY, DEFAULT_TELEGRAM_GROUP_ID);

  preferences.end();

  processBotMessages(botToken, groupId);

  if (firstLoop) {
    sendBotMessage(botToken, groupId, WELCOME_MESSAGE);
  } else {
    sendBotMessage(botToken, groupId, "Настало время кормить питомца.");
  }

  firstLoop = false;

  // Put the ESP32 to deep sleep
  ESP.deepSleep(DEEP_SLEEP_DURATION_US);
}
