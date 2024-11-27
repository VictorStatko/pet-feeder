#include "WiFiManagerWrapper.h"
#include "PreferencesHandler.h"
#include <WiFiManager.h>
#include "CollectionUtils.h"
#include <vector>
#include "TelegramHandler.h"

const char* const DEFAULT_AP_NAME = "PetFeeder";
const char* const DEFAULT_AP_PASSWORD = "11111111";
const char* const TELEGRAM_BOT_TOKEN_KEY = "botToken";
const char* const TELEGRAM_GROUP_ID_KEY = "groupId";
const char* const FEEDING_SCHEDULE_KEY = "feedingSchedule";
const char* const FEEDING_PORTION_WEIGHT_KEY = "feedingPortionWeight";
const char* const FEEDING_BOWL_WEIGHT_KEY = "feedingBowlWeight";
const int CONFIG_PORTAL_TIMEOUT_S = 300;

void WiFiManagerWrapper::setupWiFiManager() {
  Serial.println("WiFiManagerWrapper - Setting up WiFiManager");

  bool success = false;

  while (!success) {
    WiFiManager wm;
    wm.setConfigPortalTimeout(CONFIG_PORTAL_TIMEOUT_S);

    // Retrieve stored preferences for bot token and group ID
    Serial.println("WiFiManagerWrapper - Retrieving stored preferences");
    String botToken = PreferencesHandler::getBotToken();
    String groupId = PreferencesHandler::getGroupId();
    String feedingSchedule = CollectionUtils::joinVector(PreferencesHandler::getFeedingSchedule());
    int feedingWeightPerPortion = PreferencesHandler::getFeedingWeightPerPortion();
    int feedingBowlWeight = PreferencesHandler::getFeedingBowlWeight();

    Serial.println("WiFiManagerWrapper - Retrieved bot token: " + botToken);
    Serial.println("WiFiManagerWrapper - Retrieved group ID: " + groupId);
    Serial.println("WiFiManagerWrapper - Retrieved feeding schedule: " + feedingSchedule);
    Serial.println("WiFiManagerWrapper - Retrieved feeding weight per portion: " + String(feedingWeightPerPortion));
    Serial.println("WiFiManagerWrapper - Retrieved feeding bowl weight: " + String(feedingBowlWeight));

    // Add custom parameters for WiFiManager
    Serial.println("WiFiManagerWrapper - Adding custom parameters to WiFiManager");
    WiFiManagerParameter custom_bot_token(TELEGRAM_BOT_TOKEN_KEY, "Telegram Bot Token", botToken.c_str(), 64);
    WiFiManagerParameter custom_group_id(TELEGRAM_GROUP_ID_KEY, "Telegram Group ID", groupId.c_str(), 20);
    WiFiManagerParameter custom_feeding_schedule(FEEDING_SCHEDULE_KEY, "Feeding schedule UTC (ex. 10:00,15:00,20:00). Asc order, min period between feedings - 1 hour", feedingSchedule.c_str(), 70);
    WiFiManagerParameter custom_feeding_portion_weight(FEEDING_PORTION_WEIGHT_KEY, "Feeding portion weight in grams", String(feedingWeightPerPortion).c_str(), 4);
    WiFiManagerParameter custom_feeding_bowl_weight(FEEDING_BOWL_WEIGHT_KEY, "Feeding bowl weight in grams", String(feedingBowlWeight).c_str(), 4);

    Serial.println("WiFiManagerWrapper - Custom parameters added to WiFiManager");

    wm.addParameter(&custom_bot_token);
    wm.addParameter(&custom_group_id);
    wm.addParameter(&custom_feeding_schedule);
    wm.addParameter(&custom_feeding_portion_weight);
    wm.addParameter(&custom_feeding_bowl_weight);

    // Start WiFi configuration portal
    Serial.println("WiFiManagerWrapper - Starting configuration portal");
    if (!wm.startConfigPortal(DEFAULT_AP_NAME, DEFAULT_AP_PASSWORD)) {
      Serial.println("WiFiManagerWrapper - Failed to connect or configure. Continuing with existing settings.");
    } else {
      Serial.println("WiFiManagerWrapper - WiFi configuration completed successfully.");
    }

    // Save updated preferences
    Serial.println("WiFiManagerWrapper - Saving updated preferences");
    PreferencesHandler::saveBotToken(custom_bot_token.getValue());
    PreferencesHandler::saveGroupId(custom_group_id.getValue());

    success = true;

    if (!PreferencesHandler::saveFeedingSchedule(custom_feeding_schedule.getValue())) {
      success = false;
      Serial.println("WiFiManagerWrapper - Feeding schedule not valid or not set.");
      TelegramHandler::sendBotMessage("Расписание кормления не задано или не валидно. Проведите конфигурацию снова.");
    } else {
      Serial.println("WiFiManagerWrapper - Feeding schedule saved successfully.");
    }

    if (!PreferencesHandler::saveFeedingWeightPerPortion(custom_feeding_portion_weight.getValue())) {
      success = false;
      Serial.println("WiFiManagerWrapper - Feeding weight not valid or not set.");
      TelegramHandler::sendBotMessage("Вес порции не задан или не валиден. Проведите конфигурацию снова.");
    } else {
      Serial.println("WiFiManagerWrapper - Feeding weight per portion saved successfully.");
    }

    if (!PreferencesHandler::saveFeedingBowlWeight(custom_feeding_bowl_weight.getValue())) {
      success = false;
      Serial.println("WiFiManagerWrapper - Feeding bowl weight not valid or not set.");
      TelegramHandler::sendBotMessage("Вес миски не задан или не валиден. Проведите конфигурацию снова.");
    } else {
      Serial.println("WiFiManagerWrapper - Feeding bowl weight saved successfully.");
    }
  }

  Serial.println("WiFiManagerWrapper - Preferences saved");
}

void WiFiManagerWrapper::autoConnectWiFi() {
  Serial.println("WiFiManagerWrapper - Attempting to auto-connect to WiFi");
  WiFiManager wm;
  wm.setConfigPortalTimeout(1);

  int retries = 0;
  while (retries < 10) {
    if (wm.autoConnect()) {
      Serial.println("WiFiManagerWrapper - Auto-connect successful.");
      return;  // Exit if connection is successful
    } else {
      retries++;
      Serial.print("WiFiManagerWrapper - Auto-connect failed. Retry ");
      Serial.print(retries);
      Serial.println(" of 10.");
    }
  }
  Serial.println("WiFiManagerWrapper - Auto-connect failed after 10 retries. Check WiFi settings.");
}
