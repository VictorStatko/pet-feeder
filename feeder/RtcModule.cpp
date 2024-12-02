#include "RtcModule.h"
#include "UsedPins.h"
#include "Messages.h"

RtcModule::RtcModule(int dataPin, int clkPin, int rstPin, TelegramHandler& handler)
  : wire(dataPin, clkPin, rstPin), rtc(wire), telegramHandler(handler) {}

String RtcModule::getDateTimeString(const RtcDateTime& dt) {
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

time_t RtcModule::rtcToTime_t(const RtcDateTime& rtcDateTime) {
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

void RtcModule::begin() {
  rtc.Begin();
}

void RtcModule::sync() {
  Serial.println("RtcModule - Syncing RTC module...");

  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);

  if (!rtc.IsDateTimeValid()) {
    Serial.println("RtcModule - RTC lost confidence in the DateTime! Setting to compiled time.");
    rtc.SetDateTime(compiled);
    telegramHandler.sendBotMessage(MESSAGE_RTC_MODULE_ERROR);
  }

  if (rtc.GetIsWriteProtected()) {
    Serial.println("RtcModule - RTC was write protected, enabling writing now");
    rtc.SetIsWriteProtected(false);
  }

  if (!rtc.GetIsRunning()) {
    Serial.println("RtcModule - RTC was not actively running, starting now");
    rtc.SetIsRunning(true);
  }

  RtcDateTime currentRtcTime = rtc.GetDateTime();
  Serial.println("RtcModule - Current time: " + getDateTimeString(currentRtcTime));

  bool timeSynced = TimeHandler::syncRealTimeClock();

  if (timeSynced) {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
      currentRtcTime = rtc.GetDateTime();
      telegramHandler.sendBotMessage(MESSAGE_TIME_SYNC_ERROR + getDateTimeString(currentRtcTime));
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

    rtc.SetDateTime(newTime);

    currentRtcTime = rtc.GetDateTime();
    Serial.println("RtcModule - Time sync successful. RTC updated. Current time: " + getDateTimeString(currentRtcTime));
  } else {
    currentRtcTime = rtc.GetDateTime();
    telegramHandler.sendBotMessage(MESSAGE_TIME_SYNC_ERROR + getDateTimeString(currentRtcTime));
    Serial.println("RtcModule - Time sync failed, sent error message to Telegram. Current time: " + getDateTimeString(currentRtcTime));
  }
}

time_t RtcModule::getCurrentTime() {
  Serial.println("RtcModule - Retrieving current time from RTC...");
  RtcDateTime currentRtcTime = rtc.GetDateTime();

  if (!currentRtcTime.IsValid()) {
    time_t now;
    time(&now);
    telegramHandler.sendBotMessage(MESSAGE_TIME_RETRIEVING_ERROR + String(now));
    Serial.println("RtcModule - Invalid RTC time, fallback to system time.");
    return now;
  }

  Serial.print("RtcModule - Current time retrieved from RTC: ");
  Serial.println(getDateTimeString(currentRtcTime));
  return rtcToTime_t(currentRtcTime);
}
