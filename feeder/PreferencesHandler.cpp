#include <sys/_intsup.h>
#include "PreferencesHandler.h"
#include <Preferences.h>
#include "ScheduleHandler.h"

const char* const PREF_TELEGRAM = "telegram";
const char* const PREF_TELEGRAM_BOT_TOKEN_KEY = "botToken";
const char* const PREF_TELEGRAM_GROUP_ID_KEY = "groupId";
const char* const DEFAULT_TELEGRAM_BOT_TOKEN = "";
const char* const DEFAULT_TELEGRAM_GROUP_ID = "";
const char* const PREF_FEEDING = "feeding";
const char* const PREF_FEEDING_SCHEDULE = "schedule";
const char* const PREF_FEEDING_PORTION_WEIGHT = "portionWeight";
const char* const PREF_FEEDING_BOWL_WEIGHT = "bowlWeight";
const char* const PREF_FEEDING_LAST_TIME = "lastTime";
const char* const DEFAULT_FEEDING_SCHEDULE = "";
const int DEFAULT_FEEDING_WEIGHT = 0;
const int DEFAULT_FEEDING_BOWL_WEIGHT = 0;
const int DEFAULT_FEEDING_LAST_TIME = 0;

String PreferencesHandler::getBotToken() {
  Serial.println("PreferencesHandler - Reading bot token from preferences");
  Preferences preferences;
  preferences.begin(PREF_TELEGRAM, true);
  String botToken = preferences.getString(PREF_TELEGRAM_BOT_TOKEN_KEY, DEFAULT_TELEGRAM_BOT_TOKEN);
  preferences.end();
  Serial.println("PreferencesHandler - Retrieved bot token: " + botToken);
  return botToken;
}

String PreferencesHandler::getGroupId() {
  Serial.println("PreferencesHandler - Reading group ID from preferences");
  Preferences preferences;
  preferences.begin(PREF_TELEGRAM, true);
  String groupId = preferences.getString(PREF_TELEGRAM_GROUP_ID_KEY, DEFAULT_TELEGRAM_GROUP_ID);
  preferences.end();
  Serial.println("PreferencesHandler - Retrieved group ID: " + groupId);
  return groupId;
}

std::vector<String> PreferencesHandler::getFeedingSchedule() {
  Serial.println("PreferencesHandler - Reading feeding schedule from preferences");
  Preferences preferences;
  preferences.begin(PREF_FEEDING, true);
  String feedingSchedule = preferences.getString(PREF_FEEDING_SCHEDULE, DEFAULT_FEEDING_SCHEDULE);
  preferences.end();
  Serial.println("PreferencesHandler - Retrieved feeding schedule: " + feedingSchedule);

  std::vector<String> parsedSchedule;

  int startIndex = 0;
  int commaIndex = feedingSchedule.indexOf(',');

  // Split the schedule by commas
  while (commaIndex != -1) {
    parsedSchedule.push_back(feedingSchedule.substring(startIndex, commaIndex));
    startIndex = commaIndex + 1;
    commaIndex = feedingSchedule.indexOf(',', startIndex);
  }

  // Add the last time (or the only time if no commas exist)
  if (startIndex < feedingSchedule.length()) {
    parsedSchedule.push_back(feedingSchedule.substring(startIndex));
  }

  Serial.println("PreferencesHandler - Parsed feeding schedule: ");
  for (const String& time : parsedSchedule) {
    Serial.println("PreferencesHandler - " + time);
  }

  return parsedSchedule;
}

int PreferencesHandler::getFeedingWeightPerPortion() {
  Serial.println("PreferencesHandler - Reading feeding weight per portion from preferences");
  Preferences preferences;
  preferences.begin(PREF_FEEDING, true);
  int weight = preferences.getInt(PREF_FEEDING_PORTION_WEIGHT, DEFAULT_FEEDING_WEIGHT);
  preferences.end();
  Serial.println("PreferencesHandler - Retrieved feeding weight per portion weight: " + String(weight));
  return weight;
}

int PreferencesHandler::getFeedingBowlWeight() {
  Serial.println("PreferencesHandler - Reading feeding bowl weight from preferences");
  Preferences preferences;
  preferences.begin(PREF_FEEDING, true);
  int weight = preferences.getInt(PREF_FEEDING_BOWL_WEIGHT, DEFAULT_FEEDING_BOWL_WEIGHT);
  preferences.end();
  Serial.println("PreferencesHandler - Retrieved feeding bowl weight: " + String(weight));
  return weight;
}

time_t PreferencesHandler::getLastFeedingTime() {
  Serial.println("PreferencesHandler - Reading last feeding time from preferences");
  Preferences preferences;
  preferences.begin(PREF_FEEDING, true);
  uint32_t storedTime = preferences.getUInt(PREF_FEEDING_LAST_TIME, DEFAULT_FEEDING_LAST_TIME);
  preferences.end();
  Serial.println("PreferencesHandler - Retrieved last feeding time: " + String(storedTime));
  return static_cast<time_t>(storedTime);
}

void PreferencesHandler::saveBotToken(const String& botToken) {
  Serial.println("PreferencesHandler - Saving bot token to preferences: " + botToken);
  Preferences preferences;
  preferences.begin(PREF_TELEGRAM, false);
  preferences.putString(PREF_TELEGRAM_BOT_TOKEN_KEY, botToken);
  preferences.end();
  Serial.println("PreferencesHandler - Bot token saved successfully");
}

void PreferencesHandler::saveGroupId(const String& groupId) {
  Serial.println("PreferencesHandler - Saving group ID to preferences: " + groupId);
  Preferences preferences;
  preferences.begin(PREF_TELEGRAM, false);
  preferences.putString(PREF_TELEGRAM_GROUP_ID_KEY, groupId);
  preferences.end();
  Serial.println("PreferencesHandler - Group ID saved successfully");
}

bool PreferencesHandler::saveFeedingSchedule(const String& feedingSchedule) {
  Serial.println("PreferencesHandler - Saving feeding schedule to preferences: " + feedingSchedule);

  boolean valid = ScheduleHandler::validateFeedingSchedule(feedingSchedule);

  if (!valid) {
    Serial.println("PreferencesHandler - Invalid feeding schedule. Aborting save.");
    return false;
  }

  Preferences preferences;
  preferences.begin(PREF_FEEDING, false);
  preferences.putString(PREF_FEEDING_SCHEDULE, feedingSchedule);
  preferences.end();
  Serial.println("PreferencesHandler - Feeding schedule saved successfully");

  return true;
}

bool PreferencesHandler::saveFeedingWeightPerPortion(const String& weightPerPortion) {
  Serial.println("PreferencesHandler - Saving feeding weight per portion to preferences: " + weightPerPortion);

  if (weightPerPortion.length() == 0) {
    Serial.println("PreferencesHandler - Weight per portion is empty. Aborting save.");
    return false;
  }

  int weight = weightPerPortion.toInt();

  if (weight == 0 || weight < 1 || weight > 1000) {
    Serial.println("PreferencesHandler - Invalid feeding weight. Must be between 1 and 1000.");
    return false;  // Exit if the input is invalid
  }

  Preferences preferences;
  preferences.begin(PREF_FEEDING, false);
  preferences.putInt(PREF_FEEDING_PORTION_WEIGHT, weight);
  preferences.end();
  Serial.println("PreferencesHandler - Feeding weight per portion saved successfully");

  return true;
}

bool PreferencesHandler::saveFeedingBowlWeight(const String& bowlWeight) {
  Serial.println("PreferencesHandler - Saving feeding bowl weight to preferences: " + bowlWeight);

  if (bowlWeight.length() == 0) {
    Serial.println("PreferencesHandler - Bowl weight is empty. Aborting save.");
    return false;
  }

  int weight = bowlWeight.toInt();

  if (weight == 0 || weight < 1 || weight > 1000) {
    Serial.println("PreferencesHandler - Invalid feeding bowl weight. Must be between 1 and 1000.");
    return false;  // Exit if the input is invalid
  }

  Preferences preferences;
  preferences.begin(PREF_FEEDING, false);
  preferences.putInt(PREF_FEEDING_BOWL_WEIGHT, weight);
  preferences.end();
  Serial.println("PreferencesHandler - Feeding bowl weight saved successfully");

  return true;
}

void PreferencesHandler::saveLastFeedingTime(const time_t feedingTime) {
  Serial.println("PreferencesHandler - Saving last feeding time to preferences: " + String(feedingTime));

  Preferences preferences;
  preferences.begin(PREF_FEEDING, false);
  preferences.putUInt(PREF_FEEDING_LAST_TIME, static_cast<uint32_t>(feedingTime));
  preferences.end();
  Serial.println("PreferencesHandler - Feeding last feeding time saved successfully");
}
