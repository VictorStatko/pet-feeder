#include "TelegramHandler.h"
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "PreferencesHandler.h"
#include <WiFi.h>

const char* const TELEGRAM_CERT = TELEGRAM_CERTIFICATE_ROOT;
const int TELEGRAM_MAX_RETRIES = 3;
const int TELEGRAM_RETRY_DELAY_MS = 2000;

void TelegramHandler::sendBotMessage(const String& message) {
  Serial.println("TelegramHandler - Attempting to send message: " + message);

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("TelegramHandler - Wi-Fi not connected. Cannot send message.");
    return;
  }

  // Retrieve botToken and groupId from preferences
  Serial.println("TelegramHandler - Retrieving bot token and group ID from preferences.");
  String botToken = PreferencesHandler::getBotToken();
  String groupId = PreferencesHandler::getGroupId();

  if (botToken.isEmpty() || groupId.isEmpty()) {
    Serial.println("TelegramHandler - Missing bot token or group ID. Aborting message send.");
    return;
  }

  WiFiClientSecure securedClient;
  securedClient.setCACert(TELEGRAM_CERT);
  UniversalTelegramBot bot(botToken, securedClient);

  Serial.println("TelegramHandler - Initialized bot with retrieved credentials.");

  int retryCount = 0;
  bool success = false;

  while (retryCount < TELEGRAM_MAX_RETRIES && !success) {
    Serial.println("TelegramHandler - Sending message. Attempt: " + String(retryCount + 1));
    success = bot.sendMessage(groupId, message);

    if (!success) {
      Serial.println("TelegramHandler - Error sending message. Retrying...");
      retryCount++;
      delay(TELEGRAM_RETRY_DELAY_MS);
    }
  }

  if (success) {
    Serial.println("TelegramHandler - Message sent successfully.");
  } else {
    Serial.println("TelegramHandler - Failed to send message after " + String(TELEGRAM_MAX_RETRIES) + " attempts.");
  }
}
