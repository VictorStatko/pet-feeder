#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiManager.h>
#include <UniversalTelegramBot.h>
#include <Preferences.h>
#include "soc/rtc.h"

RTC_DATA_ATTR bool initialSetupDone = false;
RTC_DATA_ATTR bool firstLoop = true;

Preferences preferences;
WiFiClientSecure securedClient;

void setup() {
  WiFi.mode(WIFI_STA);

  Serial.begin(115200);

  securedClient.setCACert(TELEGRAM_CERTIFICATE_ROOT);

  if (!initialSetupDone) {
    WiFiManager wm;

    wm.setConfigPortalTimeout(300);

    preferences.begin("telegram", false);

    String botToken = preferences.getString("botToken", "");
    String groupId = preferences.getString("groupId", "");

    WiFiManagerParameter custom_bot_token("botToken", "Telegram Bot Token", botToken.c_str(), 64);
    WiFiManagerParameter custom_group_id("groupId", "Telegram Group ID", groupId.c_str(), 20);

    wm.addParameter(&custom_bot_token);
    wm.addParameter(&custom_group_id);

    wm.startConfigPortal("PetFeeder", "11111111");

    botToken = custom_bot_token.getValue();
    groupId = custom_group_id.getValue();

    preferences.putString("botToken", botToken);
    preferences.putString("groupId", groupId);

    initialSetupDone = true;
  }
}

void sendBotMessageWithRetry(const String botToken, const String chatId, const String message) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Send bot message: Wi-Fi not connected!");
    return;
  }

  if (botToken.isEmpty()) {
    Serial.println("Send bot message: bot token not provided!");
    return;
  }

  if (chatId.isEmpty()) {
    Serial.println("Send bot message: chatId not provided!");
    return;
  }

  UniversalTelegramBot bot(botToken, securedClient);

  int retryCount = 0;
  const int maxRetries = 3;
  const int retryDelay = 2000;  // 2 seconds

  bool success = false;

  while (retryCount < maxRetries && !success) {
    success = bot.sendMessage(chatId, message);

    if (!success) {
      Serial.println("Send bot message: Error. Retrying...");
      retryCount++;
      delay(retryDelay);
    }
  }

  if (!success) {
    Serial.println("Send bot message: Failed to send message after retries!");
  }
}

void loop() {
  WiFiManager wm;

  wm.setConfigPortalTimeout(0);

  wm.autoConnect();

  preferences.begin("telegram", false);

  String botToken = preferences.getString("botToken", "");
  String groupId = preferences.getString("groupId", "");

  if (firstLoop) {
    sendBotMessageWithRetry(botToken, groupId, "Устройство готово к использованию.");
  } else {
    sendBotMessageWithRetry(botToken, groupId, "Настало время кормить питомца.");
  }


  firstLoop = false;

  // Put the ESP32 to deep sleep for 1 minute
  ESP.deepSleep(60000000);  // Sleep for 1 minute (in microseconds)
}
