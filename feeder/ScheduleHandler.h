#ifndef SCHEDULE_HANDLER_H
#define SCHEDULE_HANDLER_H

#include <Arduino.h>
#include <vector>
#include <time.h>

const int TOLERANCE = 900;  // in seconds
const int SECONDS_IN_HOUR = 3600;
const int SECONDS_IN_DAY = 86400;

class ScheduleHandler {
public:
  static std::vector<time_t> parseFeedingSchedule(const time_t now);

  static time_t shouldFeedNow(const std::vector<time_t>& feedingTimes, const time_t now);

  static time_t calculateNextWakeup(const std::vector<time_t>& feedingTimes, const time_t now, const bool feedingInCurrentIteration);

  static bool validateFeedingSchedule(const String& feedingSchedule);
};

#endif
