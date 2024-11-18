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
const char* DEFAULT_TELEGRAM_BOT_TOKEN = "";
const char* DEFAULT_TELEGRAM_GROUP_ID = "";
const char* TELEGRAM_CERT = TELEGRAM_CERTIFICATE_ROOT;
const int CONFIG_PORTAL_TIMEOUT_S = 300;
const int TELEGRAM_MAX_RETRIES = 3;
const int TELEGRAM_RETRY_DELAY_MS = 2000;
const uint64_t DEEP_SLEEP_DURATION_US = 60000000;  // 1 minute in microseconds

const char* WELCOME_MESSAGE = "Устройство готово к использованию.";

// Persistent variables
RTC_DATA_ATTR bool initialSetupDone = false;
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

// Main loop
void loop() {
  WiFiManager wm;

  wm.setConfigPortalTimeout(1);
  wm.autoConnect();

  preferences.begin(PREF_TELEGRAM, true);

  String botToken = preferences.getString(PREF_TELEGRAM_BOT_TOKEN_KEY, DEFAULT_TELEGRAM_BOT_TOKEN);
  String groupId = preferences.getString(PREF_TELEGRAM_GROUP_ID_KEY, DEFAULT_TELEGRAM_GROUP_ID);

  preferences.end();

  if (firstLoop) {
    sendBotMessage(botToken, groupId, WELCOME_MESSAGE);
  } else {
    sendBotMessage(botToken, groupId, "Настало время кормить питомца.");
  }

  firstLoop = false;

  // Put the ESP32 to deep sleep
  ESP.deepSleep(DEEP_SLEEP_DURATION_US);
}
