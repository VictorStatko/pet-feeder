#include "RtcModule.h"
#include <RtcDS1302.h>
#include "UsedPins.h"
#include "TimeHandler.h"
#include <time.h>
#include "TelegramHandler.h"
#include "Messages.h"

ThreeWire myWire(RTC_MODULE_DAT_PIN, RTC_MODULE_CLK_PIN, RTC_MODULE_RST_PIN);
RtcDS1302<ThreeWire> Rtc(myWire);

String getDateTimeString(const RtcDateTime& dt) {
  char datestring[26];

  snprintf_P(
    datestring,
    countof(datestring),
    PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
    dt.Month(),
    dt.Day(),
    dt.Year(),
    dt.Hour(),
    dt.Minute(),
    dt.Second());

  return String(datestring);
}

time_t rtcToTime_t(const RtcDateTime& rtcDateTime) {
  struct tm timeinfo;
  timeinfo.tm_year = rtcDateTime.Year() - 1900;
  timeinfo.tm_mon = rtcDateTime.Month() - 1;
  timeinfo.tm_mday = rtcDateTime.Day();
  timeinfo.tm_hour = rtcDateTime.Hour();
  timeinfo.tm_min = rtcDateTime.Minute();
  timeinfo.tm_sec = rtcDateTime.Second();
  timeinfo.tm_isdst = -1;

  return mktime(&timeinfo);
}

void RtcModule::sync() {
  Serial.println("RtcModule - Syncing RTC module...");

  Rtc.Begin();
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);

  if (!Rtc.IsDateTimeValid()) {
    Serial.println("RtcModule - RTC lost confidence in the DateTime! Setting to compiled time.");
    Rtc.SetDateTime(compiled);
    TelegramHandler::sendBotMessage(MESSAGE_RTC_MODULE_ERROR);
  }

  if (Rtc.GetIsWriteProtected()) {
    Serial.println("RtcModule - RTC was write protected, enabling writing now");
    Rtc.SetIsWriteProtected(false);
  }

  if (!Rtc.GetIsRunning()) {
    Serial.println("RtcModule - RTC was not actively running, starting now");
    Rtc.SetIsRunning(true);
  }

  RtcDateTime currentRtcTime = Rtc.GetDateTime();

  Serial.println("RtcModule - Current time: " + getDateTimeString(currentRtcTime));

  bool timeSynced = TimeHandler::syncRealTimeClock();

  if (timeSynced) {
    struct tm timeinfo;

    if (!getLocalTime(&timeinfo)) {
      currentRtcTime = Rtc.GetDateTime();

      TelegramHandler::sendBotMessage(MESSAGE_TIME_SYNC_ERROR + getDateTimeString(currentRtcTime));

      Serial.println("RtcModule - Failed to obtain ESP32 RTC time. Current time: " + getDateTimeString(currentRtcTime));
      return;
    }

    RtcDateTime newTime = RtcDateTime(
      timeinfo.tm_year + 1900,
      timeinfo.tm_mon + 1,
      timeinfo.tm_mday,
      timeinfo.tm_hour,
      timeinfo.tm_min,
      timeinfo.tm_sec);

    Rtc.SetDateTime(newTime);

    currentRtcTime = Rtc.GetDateTime();

    Serial.println("RtcModule - Time sync successful. RTC updated. Current time: " + getDateTimeString(currentRtcTime));
  } else {
    currentRtcTime = Rtc.GetDateTime();

    TelegramHandler::sendBotMessage(MESSAGE_TIME_SYNC_ERROR + getDateTimeString(currentRtcTime));
    Serial.println("RtcModule - Time sync failed, sent error message to Telegram. Current time: " + getDateTimeString(currentRtcTime));
  }
}

time_t RtcModule::getCurrentTime() {
  Serial.println("RtcModule - Retrieving current time from RTC...");

  RtcDateTime currentRtcTime = Rtc.GetDateTime();

  if (!currentRtcTime.IsValid()) {
    time_t now;
    time(&now);

    TelegramHandler::sendBotMessage(MESSAGE_TIME_RETRIEVING_ERROR + String(now));

    Serial.println("RtcModule - Invalid RTC time, fallback to system time.");
    return now;
  }

  Serial.print("RtcModule - Current time retrieved from RTC: ");
  Serial.println(getDateTimeString(currentRtcTime));

  return rtcToTime_t(currentRtcTime);
}
