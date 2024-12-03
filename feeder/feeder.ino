#include <Arduino.h>
#include "WiFiManagerWrapper.h"
#include "TelegramHandler.h"
#include <WiFi.h>
#include "ScheduleHandler.h"
#include "PreferencesHandler.h"
#include "VoltageSensor.h"
#include "WeightSensor.h"
#include "Messages.h"
#include "RtcModule.h"
#include "UsedPins.h"
#include <cmath>

const uint64_t DEEP_SLEEP_DURATION_US = 60000000;

// Persistent variables
RTC_DATA_ATTR bool initialSetupDone = false;

TelegramHandler telegramHandler;
RtcModule rtcModule(RTC_MODULE_DAT_PIN, RTC_MODULE_CLK_PIN, RTC_MODULE_RST_PIN, telegramHandler);
VoltageSensor espVoltageSensor(ESP_PART_VOLTAGE_SENSOR_PIN, 0.966);
VoltageSensor motorVoltageSensor(MOTOR_PART_VOLTAGE_SENSOR_PIN, 0.958);
WeightSensor weightSensor(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN, 1106, -186450);

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
  weightSensor.begin();

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

    int bowlWeight = PreferencesHandler::getFeedingBowlWeight();
    int weightPerPortion = PreferencesHandler::getFeedingWeightPerPortion();
    float currentWeight = weightSensor.readWeight();

    if (std::isnan(currentWeight)) {
      Serial.println("Main - Error: Weight sensor not ready");
      telegramHandler.sendBotMessage("Ошибка при кормлении");
    } else {
      if (currentWeight < bowlWeight / 2) {
        Serial.print("Main - No bowl. Feeding will not be executed.");
        telegramHandler.sendBotMessage("На весах нет миски, кормление пропущено.");
      } else {
        float adjustedWeight = currentWeight - bowlWeight;

        if (adjustedWeight < 0) {
          adjustedWeight = 0;
        }

        Serial.print("Main - Current wieght of food: ");
        Serial.println(adjustedWeight);

        telegramHandler.sendBotMessage("Вес еды в миске: " + String(adjustedWeight));

        float weightToFeed = weightPerPortion - adjustedWeight;

        if (weightToFeed <= 0) {
          Serial.println("Main - Current wieght of food is enough. Feeding will be skipped");
          telegramHandler.sendBotMessage("Еды достаточно. Кормление будет пропущено");
        } else {
          Serial.print("Main - Need to add the following amount of food (in grams): ");
          Serial.println(weightToFeed);
          telegramHandler.sendBotMessage("Будет добавлено данное количество корма (в граммах): " + String(weightToFeed));
        }
      }
    }

    PreferencesHandler::saveLastFeedingTime(feedingTime);
    rtcModule.sync();
  } else {
    Serial.println("Main - Not feeding time yet.");
  }

  weightSensor.end();

  goToDeepSleep(nextWakeup - now);
}
