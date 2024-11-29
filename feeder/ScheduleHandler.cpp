#include "ScheduleHandler.h"
#include "PreferencesHandler.h"
#include "TimeUtils.h"

const int MIN_TIME_GAP = 120;

std::vector<time_t> ScheduleHandler::parseFeedingSchedule(const time_t now) {
  std::vector<String> feedingSchedule = PreferencesHandler::getFeedingSchedule();
  std::vector<time_t> feedingTimes;

  struct tm timeinfo;

  localtime_r(&now, &timeinfo);

  int year = timeinfo.tm_year;
  int month = timeinfo.tm_mon;
  int day = timeinfo.tm_mday;

  Serial.println("ScheduleHandler - Parsing feeding schedule...");

  for (String timeStr : feedingSchedule) {
    int hour, minute;
    sscanf(timeStr.c_str(), "%d:%d", &hour, &minute);
    timeinfo.tm_year = year;
    timeinfo.tm_mon = month;
    timeinfo.tm_mday = day;
    timeinfo.tm_hour = hour;
    timeinfo.tm_min = minute;
    timeinfo.tm_sec = 0;

    feedingTimes.push_back(mktime(&timeinfo));
    Serial.println("ScheduleHandler - Parsed feeding time: " + String(hour) + ":" + String(minute));
  }

  return feedingTimes;
}

time_t ScheduleHandler::shouldFeedNow(const std::vector<time_t>& feedingTimes, const time_t now) {
  time_t lastFeedingTime = PreferencesHandler::getLastFeedingTime();

  time_t previousDayFeedingTime = feedingTimes.back() - SECONDS_IN_DAY;  // Check last feeding of previous day (time drift)

  if (now - previousDayFeedingTime <= TOLERANCE && previousDayFeedingTime != lastFeedingTime) {
    Serial.println("ScheduleHandler - Feeding time from the previous day within tolerance.");
    return previousDayFeedingTime;
  }

  for (time_t feedingTime : feedingTimes) {
    if (feedingTime > now - TOLERANCE && feedingTime < now + TOLERANCE && feedingTime != lastFeedingTime) {
      Serial.println("ScheduleHandler - Should feed now at: " + String(feedingTime));
      return feedingTime;
    }
  }

  Serial.println("ScheduleHandler - No feeding time within tolerance.");
  return 0;
}

time_t ScheduleHandler::calculateNextWakeup(const std::vector<time_t>& feedingTimes, const time_t now) {
  Serial.println("ScheduleHandler - Calculating next wakeup time...");

  // Iterate through the feeding times and return the next valid wakeup time
  for (time_t feedingTime : feedingTimes) {
    if (feedingTime > now) {
      // If the feeding time is more than an hour away, wake up in 1 hour
      if (feedingTime - now > SECONDS_IN_HOUR) {
        Serial.println("ScheduleHandler - Wake up in 1 hour.");
        return now + SECONDS_IN_HOUR;
      }
      Serial.println("ScheduleHandler - Wake up at: " + String(feedingTime));
      return feedingTime;  // Otherwise, return the actual feeding time
    }
  }

  // If all feeding times are in the past, check the next day's feeding time
  time_t nextDayFeedingTime = feedingTimes[0] + SECONDS_IN_DAY;
  // If the next day's feeding time is more than an hour away, wake up in 1 hour
  if (nextDayFeedingTime - now > SECONDS_IN_HOUR) {
    Serial.println("ScheduleHandler - Wake up in 1 hour.");
    return now + SECONDS_IN_HOUR;
  }

  Serial.println("ScheduleHandler - Wake up at: " + String(nextDayFeedingTime));
  return nextDayFeedingTime;  // Otherwise, return the next day's feeding time
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
bool ScheduleHandler::validateFeedingSchedule(const String& feedingSchedule) {
  if (feedingSchedule.length() == 0) {
    Serial.println("ScheduleHandler - Feeding schedule is empty.");
    return false;
  }

  // Check for invalid characters (only digits, commas, and colon are allowed)
  for (int i = 0; i < feedingSchedule.length(); i++) {
    char ch = feedingSchedule.charAt(i);
    if (!isValidTimeArrayCharacter(ch)) {
      Serial.println("ScheduleHandler - Invalid character detected: " + String(ch));
      return false;
    }
  }

  // Ensure no trailing comma
  if (hasTrailingComma(feedingSchedule)) {
    Serial.println("ScheduleHandler - Feeding schedule has a trailing comma.");
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
      Serial.println("ScheduleHandler - Invalid time format detected: " + time);
      return false;
    }

    if (previousTime.length() > 0 && !TimeUtils::areTimesAtLeastGapApart(previousTime, time, MIN_TIME_GAP)) {
      Serial.println("ScheduleHandler - Feeding times not at least " + String(MIN_TIME_GAP) + " minutes apart.");
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
    Serial.println("ScheduleHandler - Invalid time format detected: " + finalTime);
    return false;
  }

  if (previousTime.length() > 0 && !TimeUtils::areTimesAtLeastGapApart(previousTime, finalTime, MIN_TIME_GAP)) {
    Serial.println("ScheduleHandler - Feeding times not at least " + String(MIN_TIME_GAP) + " minutes apart.");
    return false;  // Return if times are not at least the specified time gap apart
  }

  // Now we need to check if the gap between the first and last time is at least the minimum time gap (check midnight)
  int lastTimeMinutes = TimeUtils::timeToMinutes(finalTime);
  int firstTimeMinutes = TimeUtils::timeToMinutes(firstTime);

  int gap = (firstTimeMinutes + (1440 - lastTimeMinutes));  // 1440 is the total minutes in a day
  if (gap < MIN_TIME_GAP) {                                 // Using MIN_TIME_GAP constant
    Serial.println("ScheduleHandler - Less than " + String(MIN_TIME_GAP) + " minutes between the first and last time in the schedule.");
    return false;  // Return if the gap is less than the minimum time gap across midnight
  }

  Serial.println("ScheduleHandler - Feeding schedule validated successfully.");
  return true;
}
