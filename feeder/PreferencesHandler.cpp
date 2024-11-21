#include <sys/_intsup.h>
#include "PreferencesHandler.h"
#include <Preferences.h>
#include "TimeUtils.h"

const char* const PREF_TELEGRAM = "telegram";
const char* const PREF_TELEGRAM_BOT_TOKEN_KEY = "botToken";
const char* const PREF_TELEGRAM_GROUP_ID_KEY = "groupId";
const char* const DEFAULT_TELEGRAM_BOT_TOKEN = "";
const char* const DEFAULT_TELEGRAM_GROUP_ID = "";
const char* const PREF_FEEDING = "feeding";
const char* const PREF_FEEDING_SCHEDULE = "schedule";
const char* const PREF_FEEDING_PORTION_WEIGHT = "portionWeight";
const char* const PREF_FEEDING_BOWL_WEIGHT = "bowlWeight";
const char* const DEFAULT_FEEDING_SCHEDULE = "";
const int DEFAULT_FEEDING_WEIGHT = 0;
const int DEFAULT_FEEDING_BOWL_WEIGHT = 0;
const int MIN_TIME_GAP = 120;

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

bool isValidTimeArrayCharacter(char ch) {
  return isDigit(ch) || ch == ':' || ch == ',';
}

bool hasTrailingComma(const String& feedingSchedule) {
  return feedingSchedule.charAt(feedingSchedule.length() - 1) == ',';
}

// This function validates the feeding schedule string, ensuring it follows the correct format and logic.
// The feeding schedule is expected to be a comma-separated list of times (HH:MM), with no trailing commas.
// It performs the following checks:
// 1. Ensures the feeding schedule is not empty.
// 2. Verifies that only valid characters (digits, commas, and colons) are present.
// 3. Confirms there is no trailing comma at the end of the string.
// 4. Validates each time in the schedule, ensuring it's in the correct HH:MM format and within valid ranges (00:00-23:59).
// 5. Ensures that consecutive feeding times are at least a minimum time gap apart (defined by MIN_TIME_GAP).
// 6. Verifies that the gap between the first and last time in the schedule is at least the minimum time gap, accounting for crossing midnight (24-hour format).
// If any of these checks fail, the function returns false, indicating an invalid schedule.
// If all checks pass, it returns true, indicating the feeding schedule is valid.
bool validateFeedingSchedule(const String& feedingSchedule) {
  if (feedingSchedule.length() == 0) {
    Serial.println("PreferencesHandler - Feeding schedule is empty.");
    return false;
  }

  // Check for invalid characters (only digits, commas, and colon are allowed)
  for (int i = 0; i < feedingSchedule.length(); i++) {
    char ch = feedingSchedule.charAt(i);
    if (!isValidTimeArrayCharacter(ch)) {
      Serial.println("PreferencesHandler - Invalid character detected: " + String(ch));
      return false;
    }
  }

  // Ensure no trailing comma
  if (hasTrailingComma(feedingSchedule)) {
    Serial.println("PreferencesHandler - Feeding schedule has a trailing comma.");
    return false;
  }

  // Split the feeding schedule string by commas
  int start = 0;
  int end = feedingSchedule.indexOf(',');
  String previousTime = "";  // To store the last valid time for comparison
  String firstTime = "";

  while (end != -1) {
    String time = feedingSchedule.substring(start, end);

    if (previousTime.length() == 0) {
      firstTime = time;  // The first time
    }

    if (!TimeUtils::isValidTimeFormat(time)) {
      return false;
    }

    if (previousTime.length() > 0 && !TimeUtils::areTimesAtLeastGapApart(previousTime, time, MIN_TIME_GAP)) {
      return false;  // Return if times are not at least the specified time gap apart
    }

    previousTime = time;
    start = end + 1;
    end = feedingSchedule.indexOf(',', start);
  }

  // Check the last time segment after the last comma
  String finalTime = feedingSchedule.substring(start);

  if (previousTime.length() == 0) {
    firstTime = finalTime;  // The first time
  }

  if (!TimeUtils::isValidTimeFormat(finalTime)) {
    return false;
  }

  if (previousTime.length() > 0 && !TimeUtils::areTimesAtLeastGapApart(previousTime, finalTime, MIN_TIME_GAP)) {
    return false;  // Return if times are not at least the specified time gap apart
  }

  // Now we need to check if the gap between the first and last time is at least the minimum time gap (check midnight)
  int lastTimeMinutes = TimeUtils::timeToMinutes(finalTime);
  int firstTimeMinutes = TimeUtils::timeToMinutes(firstTime);

  int gap = (firstTimeMinutes + (1440 - lastTimeMinutes));  // 1440 is the total minutes in a day
  if (gap < MIN_TIME_GAP) {                                 // Using MIN_TIME_GAP constant
    Serial.println("PreferencesHandler - Less than " + String(MIN_TIME_GAP) + " minutes between the first and last time in the schedule.");
    return false;  // Return if the gap is less than the minimum time gap across midnight
  }

  return true;
}


bool PreferencesHandler::saveFeedingSchedule(const String& feedingSchedule) {
  Serial.println("PreferencesHandler - Saving feeding schedule to preferences: " + feedingSchedule);

  boolean valid = validateFeedingSchedule(feedingSchedule);

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
