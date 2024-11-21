#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <Arduino.h>

class TimeUtils {
public:
  static bool isValidTimeFormat(const String& time);
  static int timeToMinutes(const String& time);
  static bool areTimesAtLeastGapApart(const String& previousTime, const String& currentTime, int minTimeGapMinutes);
};

#endif
