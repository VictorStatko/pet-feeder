#include "TimeUtils.h"

bool TimeUtils::isValidTimeFormat(const String& time) {
  // Check if the time string has a length of 5 (HH:MM)
  if (time.length() != 5 || time.charAt(2) != ':') {
    Serial.println("PreferencesHandler - Invalid time format: " + time);
    return false;
  }

  // Check if the hours and minutes parts are valid numbers
  int hours = time.substring(0, 2).toInt();
  int minutes = time.substring(3, 5).toInt();

  // Validate hours (0-23) and minutes (0-59)
  if (hours < 0 || hours > 23 || minutes < 0 || minutes > 59) {
    Serial.println("PreferencesHandler - Invalid time format (out of range): " + time);
    return false;
  }

  return true;
}

int TimeUtils::timeToMinutes(const String& time) {
  int hours = time.substring(0, 2).toInt();
  int minutes = time.substring(3, 5).toInt();
  return hours * 60 + minutes;
}

bool TimeUtils::areTimesAtLeastGapApart(const String& previousTime, const String& currentTime, int minTimeGapMinutes) {
  int prevMinutes = timeToMinutes(previousTime);
  int currMinutes = timeToMinutes(currentTime);

  if (currMinutes <= prevMinutes || currMinutes - prevMinutes < minTimeGapMinutes) {
    return false;
  }

  return true;
}