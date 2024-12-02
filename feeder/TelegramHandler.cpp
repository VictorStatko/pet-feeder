#include "TelegramHandler.h"
#include <WiFi.h>

// Constants for retries and delay
const int MAX_RETRIES = 3;
const int RETRY_DELAY_MS = 2000;
const char* const TELEGRAM_CERT = TELEGRAM_CERTIFICATE_ROOT;  // Certificate for secure communication

TelegramHandler::TelegramHandler()
  : bot(nullptr) {}

void TelegramHandler::begin(const String& botToken, const String& groupId) {
  this->botToken = botToken;
  this->groupId = groupId;

  // Check if token or group ID is missing
  if (botToken.isEmpty() || groupId.isEmpty()) {
    Serial.println("TelegramHandler - Bot token or group ID is missing.");
    return;
  }

  // Create a new bot instance with the new credentials
  if (bot != nullptr) {
    delete bot;  // Delete the old bot instance if it exists
  }

  // Initialize secured client with certificate
  securedClient.setCACert(TELEGRAM_CERT);

  // Create a new instance of UniversalTelegramBot with the updated botToken
  bot = new UniversalTelegramBot(botToken, securedClient);

  Serial.println("TelegramHandler - initialized with new botToken and groupId.");
}

// Function to send a message to the specified Telegram group
void TelegramHandler::sendBotMessage(const String& message) {

  if (bot == nullptr) {
    Serial.println("TelegramHandler - Telegram bot not initialized.");
    return;
  }

  Serial.println("Attempting to send message: " + message);

  // Check if Wi-Fi is connected before attempting to send the message
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("TelegramHandler - Wi-Fi not connected. Cannot send message.");
    return;
  }

  int retryCount = 0;
  bool success = false;

  // Retry logic for sending the message
  while (retryCount < MAX_RETRIES && !success) {
    Serial.println("TelegramHandler - Sending message. Attempt: " + String(retryCount + 1));
    success = bot->sendMessage(groupId, message);

    if (!success) {
      Serial.println("TelegramHandler - Error sending message. Retrying...");
      retryCount++;
      delay(RETRY_DELAY_MS);  // Delay before retrying
    }
  }

  if (success) {
    Serial.println("TelegramHandler - Message sent successfully.");
  } else {
    Serial.println("TelegramHandler - Failed to send message after " + String(MAX_RETRIES) + " attempts.");
  }
}
