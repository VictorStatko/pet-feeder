#ifndef RTC_MODULE_H
#define RTC_MODULE_H

#include <Arduino.h>
#include <RtcDS1302.h>
#include <time.h>
#include "TimeHandler.h"
#include "TelegramHandler.h"

class RtcModule {
private:
  ThreeWire wire;
  RtcDS1302<ThreeWire> rtc;
  TelegramHandler& telegramHandler;

  String getDateTimeString(const RtcDateTime& dt);
  time_t rtcToTime_t(const RtcDateTime& rtcDateTime);

public:
  RtcModule(int dataPin, int clkPin, int rstPin, TelegramHandler& handler);
  void begin();
  void sync();
  time_t getCurrentTime();
};

#endif
