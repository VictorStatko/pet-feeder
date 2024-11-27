#include <Arduino.h>
#include "WiFiManagerWrapper.h"
#include "TelegramHandler.h"
#include "TimeHandler.h"
#include <WiFi.h>
#include "ScheduleHandler.h"
#include "PreferencesHandler.h"

const uint64_t DEEP_SLEEP_DURATION_US = 60000000;

// Persistent variables
RTC_DATA_ATTR bool initialSetupDone = false;

// Setup function
void setup() {
  Serial.begin(115200);
  delay(1000);  // Allow Serial to stabilize
  Serial.println("Main - Device is waking up...");

  if (!initialSetupDone) {
    Serial.println("Main - Initial setup not done. Launching WiFiManager...");
    WiFiManagerWrapper::setupWiFiManager();
    initialSetupDone = true;
    Serial.println("Main - WiFiManager setup completed.");
    WiFiManagerWrapper::autoConnectWiFi();
    TelegramHandler::sendBotMessage("Устройство готово к использованию.");
  } else {
    Serial.println("Main - Initial setup already done. Skipping WiFiManager setup.");
  }
}

void goToDeepSleep(uint64_t seconds) {
  Serial.println("Main - Going to deep sleep for " + String(seconds) + " seconds...");
  delay(5000);                                       // For serial
  esp_sleep_enable_timer_wakeup(seconds * 1000000);  // Convert seconds to microseconds
  esp_deep_sleep_start();
}

// Main loop
void loop() {
  WiFiManagerWrapper::autoConnectWiFi();

  bool timeSynced = TimeHandler::syncRealTimeClock();
  if (!timeSynced) {
    Serial.println("Main - Failed to sync time. Sending Telegram message.");
    TelegramHandler::sendBotMessage("Возникла ошибка при синхронизации времени. Переподключение через 5 минут.");
    goToDeepSleep(300);
  }

  std::vector<time_t> feedingTimes = ScheduleHandler::parseFeedingSchedule();
  Serial.print("Main - Parsed feeding schedule: ");
  for (time_t feedingTime : feedingTimes) {
    Serial.print(feedingTime);
    Serial.print(" ");
  }
  Serial.println();

  time_t feedingTime = ScheduleHandler::shouldFeedNow(feedingTimes);
  if (feedingTime != 0) {
    Serial.println("Main - Feeding time! Activating feeder...");
    TelegramHandler::sendBotMessage("Настало время кормить питомца.");
    // Perform feeding action here

    PreferencesHandler::saveLastFeedingTime(feedingTime);
  } else {
    Serial.println("Main - Not feeding time yet.");
  }

  time_t nextWakeup = ScheduleHandler::calculateNextWakeup(feedingTimes);
  time_t now;
  time(&now);
  Serial.print("Main - Next wake-up time: ");
  Serial.println(nextWakeup);

  goToDeepSleep(nextWakeup - now);
}
