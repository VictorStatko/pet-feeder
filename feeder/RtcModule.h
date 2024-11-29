#ifndef RTC_MODULE_H
#define RTC_MODULE_H

#include <Arduino.h>

class RtcModule {
public:
  static void sync();
  static time_t getCurrentTime();
};

#endif
