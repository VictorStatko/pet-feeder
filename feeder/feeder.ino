#include <Arduino.h>
#include "WiFiManagerWrapper.h"
#include "TelegramHandler.h"
#include <WiFi.h>
#include "ScheduleHandler.h"
#include "PreferencesHandler.h"
#include "VoltageSensor.h"
#include "WeightSensor.h"
#include "DCMotor.h"
#include "Messages.h"
#include "RtcModule.h"
#include "UsedPins.h"
#include <cmath>

// Persistent variables
RTC_DATA_ATTR bool initialSetupDone = false;

TelegramHandler telegramHandler;
RtcModule rtcModule(RTC_MODULE_DAT_PIN, RTC_MODULE_CLK_PIN, RTC_MODULE_RST_PIN, telegramHandler);
VoltageSensor voltageSensor(VOLTAGE_SENSOR_PIN);
WeightSensor weightSensor(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN, 1106, -62230);
DCMotor dcMotor(MOTOR_IN1_PIN, MOTOR_IN2_PIN, MOTOR_STBY_PIN);

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
    Serial.println("Main - WiFi connection attempt executed.");

    rtcModule.sync();
    Serial.println("Main - RTC synchronization step executed.");

    telegramHandler.sendBotMessage(MESSAGE_READY_TO_USE);
    telegramHandler.sendBotMessage(
      MESSAGE_CURRENT_SETTINGS + "\n" + MESSAGE_CURRENT_SETTINGS_SCHEDULING + PreferencesHandler::getFeedingScheduleString()
      + "\n" + MESSAGE_CURRENT_SETTINGS_PORTION_WEIGHT + String(PreferencesHandler::getFeedingWeightPerPortion())
      + "\n" + MESSAGE_CURRENT_SETTINGS_BOWL_WEIGHT + String(PreferencesHandler::getFeedingBowlWeight()));
    telegramHandler.sendBotMessage(voltageSensor.getVoltageInfoMessage());

    initialSetupDone = true;
    Serial.println("Main - Initial setup completed and marked as done.");
  } else {
    Serial.println("Main - Initial setup already done. Skipping.");
  }
}

void goToDeepSleep(uint64_t seconds) {
  Serial.println("Main - Going to deep sleep for " + String(seconds) + " seconds...");
  delay(5000);                                       // For serial
  esp_sleep_enable_timer_wakeup(seconds * 1000000);  // Convert seconds to microseconds
  esp_deep_sleep_start();
}

int feedFood(int weightToFeed, int currentWeight) {
  Serial.println("Main - Starting food feeding process...");
  int previousWeight = currentWeight;
  int newWeight = -1;
  float newWeightFloat = -1;
  unsigned long lastWeightChangeTime = millis();

  bool weightSensorError = false;
  bool noWeightChangeError = false;
  bool success = false;

  while (true) {
    dcMotor.startMotor(false);
    delay(300);
    dcMotor.startMotor(true);
    delay(150);
    dcMotor.stopMotor();
    delay(1000);

    newWeightFloat = weightSensor.readWeight();

    if (std::isnan(newWeightFloat)) {
      Serial.println("Main - Error: Weight sensor not ready.");
      weightSensorError = true;
      break;
    } else {
      newWeight = floor(newWeightFloat);
    }

    if (newWeight > previousWeight + 2) {
      lastWeightChangeTime = millis();
      previousWeight = newWeight;
      Serial.println("Main - Weight increased. Updated previous weight: " + String(previousWeight));
    }

    if (millis() - lastWeightChangeTime > 300000) {
      Serial.println("Main - Stopping motor: No significant weight increase in the last 300 seconds.");
      noWeightChangeError = true;
      break;
    }

    if (newWeight >= currentWeight + weightToFeed) {
      Serial.println("Main - Stopping motor: Target weight reached.");
      success = true;
      break;
    }
  }

  if (weightSensorError) {
    telegramHandler.sendBotMessage(MESSAGE_WEIGHT_ERROR + MESSAGE_FEEDING_STOPPED);
    return -1;
  }

  if (noWeightChangeError) {
    telegramHandler.sendBotMessage(MESSAGE_FEEDING_NO_WEIGHT_CHANGE);
  }

  if (success) {
    telegramHandler.sendBotMessage(MESSAGE_FEEDING_SUCCESS);
  }

  return newWeight;
}

void startFeeding(time_t feedingTime) {
  Serial.println("Main - Feeding time! Activating feeder...");
  weightSensor.begin();
  dcMotor.begin();

  WiFiManagerWrapper::autoConnectWiFi();
  Serial.println("Main - WiFi connection attempt executed.");

  telegramHandler.sendBotMessage(voltageSensor.getVoltageInfoMessage());
  telegramHandler.sendBotMessage(MESSAGE_TIME_TO_FEED);

  int bowlWeight = PreferencesHandler::getFeedingBowlWeight();
  int weightPerPortion = PreferencesHandler::getFeedingWeightPerPortion();
  float initialWeightFloat = weightSensor.readWeight();

  if (std::isnan(initialWeightFloat)) {
    Serial.println("Main - Error: Weight sensor not ready.");
    telegramHandler.sendBotMessage(MESSAGE_WEIGHT_ERROR + MESSAGE_FEEDING_MISSED);
  } else {
    int initialWeight = floor(initialWeightFloat);
    int adjustedWeight = max(0, initialWeight - bowlWeight);

    if (initialWeight < bowlWeight / 2) {
      Serial.println("Main - No bowl. Feeding will not be executed.");
      telegramHandler.sendBotMessage(MESSAGE_NO_BOWL);
    } else {
      Serial.println("Main - Current weight of food: " + String(adjustedWeight));
      telegramHandler.sendBotMessage("Вес еды в миске: " + String(adjustedWeight) + " " + MESSAGE_GRAMM);

      int weightToFeed = weightPerPortion - adjustedWeight;

      if (weightToFeed <= 0) {
        Serial.println("Main - Current weight of food is enough. Feeding will be skipped.");
        telegramHandler.sendBotMessage(MESSAGE_FEEDING_ENOUGH_FOOD);
      } else {
        Serial.println("Main - Need to add the following amount of food (in grams): " + String(weightToFeed));
        telegramHandler.sendBotMessage(MESSAGE_FEEDING_START + String(weightToFeed) + " " + MESSAGE_GRAMM);

        int newWeight = feedFood(weightToFeed, initialWeight);

        if (newWeight != -1) {
          telegramHandler.sendBotMessage("Вес еды в миске: " + String(newWeight - initialWeight + adjustedWeight) + " " + MESSAGE_GRAMM);
        }
      }
    }
  }

  PreferencesHandler::saveLastFeedingTime(feedingTime);
  Serial.println("Main - Last feeding time saved.");

  weightSensor.end();
  dcMotor.end();
  Serial.println("Main - Feeding process completed and hardware turned off.");
}

// Main loop
void loop() {
  Serial.println("Main - Checking schedule for feeding time...");
  time_t now = rtcModule.getCurrentTime();

  std::vector<time_t> feedingTimes = ScheduleHandler::parseFeedingSchedule(now);
  time_t feedingTime = ScheduleHandler::shouldFeedNow(feedingTimes, now);

  bool feedNow = feedingTime != 0;

  time_t nextWakeup = ScheduleHandler::calculateNextWakeup(feedingTimes, now, feedNow);

  if (feedNow) {
    Serial.println("Main - Feeding time detected. Starting feeding process...");
    startFeeding(feedingTime);
    rtcModule.sync();
  } else {
    Serial.println("Main - Not feeding time yet. Next wakeup scheduled.");
  }

  goToDeepSleep(nextWakeup - now);
}
