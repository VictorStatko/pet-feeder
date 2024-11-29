#include <Arduino.h>
#include "WiFiManagerWrapper.h"
#include "TelegramHandler.h"
#include "TimeHandler.h"
#include <WiFi.h>
#include "ScheduleHandler.h"
#include "PreferencesHandler.h"
#include "VoltageSensor.h"
#include "Messages.h"
#include "RtcModule.h"

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
    Serial.println("Main - WiFiManager setup completed.");

    WiFiManagerWrapper::autoConnectWiFi();

    RtcModule::sync();

    TelegramHandler::sendBotMessage(MESSAGE_READY_TO_USE + MESSAGE_END_SEPARATOR + VoltageSensor::getVoltageInfoMessage());

    initialSetupDone = true;
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
  time_t now = RtcModule::getCurrentTime();

  std::vector<time_t> feedingTimes = ScheduleHandler::parseFeedingSchedule(now);

  time_t feedingTime = ScheduleHandler::shouldFeedNow(feedingTimes, now);

  time_t nextWakeup = ScheduleHandler::calculateNextWakeup(feedingTimes, now);

  if (feedingTime != 0) {
    Serial.println("Main - Feeding time! Activating feeder...");

    WiFiManagerWrapper::autoConnectWiFi();

    TelegramHandler::sendBotMessage(MESSAGE_TIME_TO_FEED + MESSAGE_END_SEPARATOR + VoltageSensor::getVoltageInfoMessage());
    // Perform feeding action here

    PreferencesHandler::saveLastFeedingTime(feedingTime);
    RtcModule::sync();
  } else {
    Serial.println("Main - Not feeding time yet.");
  }

  goToDeepSleep(nextWakeup - now);
}
