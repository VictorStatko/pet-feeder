#include <Arduino.h>
#include "WiFiManagerWrapper.h"
#include "TelegramHandler.h"
#include <WiFi.h>
#include "ScheduleHandler.h"
#include "PreferencesHandler.h"
#include "VoltageSensor.h"
#include "Messages.h"
#include "RtcModule.h"
#include "UsedPins.h"

const uint64_t DEEP_SLEEP_DURATION_US = 60000000;

// Persistent variables
RTC_DATA_ATTR bool initialSetupDone = false;

TelegramHandler telegramHandler;
RtcModule rtcModule(RTC_MODULE_DAT_PIN, RTC_MODULE_CLK_PIN, RTC_MODULE_RST_PIN, telegramHandler);
VoltageSensor espVoltageSensor(ESP_PART_VOLTAGE_SENSOR_PIN, 0.966);
VoltageSensor motorVoltageSensor(MOTOR_PART_VOLTAGE_SENSOR_PIN, 0.958);

String getVoltageInfoMessage() {
  return MESSAGE_BATTERY_PERCENTAGE + " - " + espVoltageSensor.getVoltageInfoMessage("ESP") + ", " + motorVoltageSensor.getVoltageInfoMessage("MOTOR");
}

// Setup function
void setup() {
  Serial.begin(115200);
  delay(1000);  // Allow Serial to stabilize
  Serial.println("Main - Device is waking up...");

  telegramHandler.begin(PreferencesHandler::getBotToken(), PreferencesHandler::getGroupId());
  rtcModule.begin();

  if (!initialSetupDone) {
    Serial.println("Main - Initial setup not done. Launching WiFiManager...");

    WiFiManagerWrapper::setupWiFiManager(telegramHandler);
    Serial.println("Main - WiFiManager setup completed.");

    WiFiManagerWrapper::autoConnectWiFi();

    rtcModule.sync();

    telegramHandler.sendBotMessage(MESSAGE_READY_TO_USE + MESSAGE_END_SEPARATOR + getVoltageInfoMessage());

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
  time_t now = rtcModule.getCurrentTime();

  std::vector<time_t> feedingTimes = ScheduleHandler::parseFeedingSchedule(now);

  time_t feedingTime = ScheduleHandler::shouldFeedNow(feedingTimes, now);

  time_t nextWakeup = ScheduleHandler::calculateNextWakeup(feedingTimes, now);

  if (feedingTime != 0) {
    Serial.println("Main - Feeding time! Activating feeder...");

    WiFiManagerWrapper::autoConnectWiFi();

    telegramHandler.sendBotMessage(MESSAGE_TIME_TO_FEED + MESSAGE_END_SEPARATOR + getVoltageInfoMessage());
    // Perform feeding action here

    PreferencesHandler::saveLastFeedingTime(feedingTime);
    rtcModule.sync();
  } else {
    Serial.println("Main - Not feeding time yet.");
  }

  goToDeepSleep(nextWakeup - now);
}
