#include <Arduino.h>
#include "WiFiManagerWrapper.h"
#include "TelegramHandler.h"
#include <WiFi.h>

const uint64_t DEEP_SLEEP_DURATION_US = 60000000;

// Persistent variables
RTC_DATA_ATTR bool initialSetupDone = false;
RTC_DATA_ATTR bool firstLoop = true;

// Setup function
void setup() {
  WiFi.mode(WIFI_STA);
  Serial.begin(115200);
  delay(1000);  // Allow Serial to stabilize
  Serial.println("Main - Device is waking up...");

  if (!initialSetupDone) {
    Serial.println("Main - Initial setup not done. Launching WiFiManager...");
    WiFiManagerWrapper::setupWiFiManager();
    initialSetupDone = true;
    Serial.println("Main - WiFiManager setup completed.");
  } else {
    Serial.println("Main - Initial setup already done. Skipping WiFiManager setup.");
  }
}

// Main loop
void loop() {
  Serial.println("Main - Attempting to auto-connect WiFi...");
  WiFiManagerWrapper::autoConnectWiFi();
  Serial.println("Main - WiFi connected successfully.");

  if (firstLoop) {
    Serial.println("Main - Sending first-time setup message to Telegram...");
    TelegramHandler::sendBotMessage("Устройство готово к использованию.");
    Serial.println("Main - First-time setup message sent.");
  } else {
    Serial.println("Main - Sending periodic message to Telegram...");
    TelegramHandler::sendBotMessage("Настало время кормить питомца.");
    Serial.println("Main - Periodic message sent.");
  }

  firstLoop = false;
  Serial.println("Main - Entering deep sleep...");
  ESP.deepSleep(DEEP_SLEEP_DURATION_US);
}
