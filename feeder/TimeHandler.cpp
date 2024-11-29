#include "TimeHandler.h"
#include <WiFi.h>
#include <time.h>
#include "sntp.h"

const char* NTP_SERVER = "pool.ntp.org";
const long GMT_OFFSET_SEC = 0;
const int DAYLIGHT_OFFSET_SEC = 0;
const int RETRIES = 60;

bool TIME_SYNCED = false;

static void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("TimeHandler - Failed to obtain time.");
    return;
  }

  char timeString[64];
  strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", &timeinfo);
  Serial.printf("TimeHandler - Current Time: %s\n", timeString);
}

void timeAvailable(struct timeval* t) {
  Serial.println("TimeHandler - Got time adjustment from NTP!");
  TIME_SYNCED = true;
}

bool TimeHandler::syncRealTimeClock() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("TimeHandler - Wi-Fi not connected. Cannot sync time.");
    return false;
  }

  Serial.println("TimeHandler - Configuring time synchronization settings...");

  sntp_set_time_sync_notification_cb(timeAvailable);
  sntp_set_sync_mode(SNTP_SYNC_MODE_IMMED);

  TIME_SYNCED = false;

  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);

  Serial.println("TimeHandler - Synchronizing time with NTP server...");

  int retryCount = 0;

  struct tm timeinfo;

  while (!TIME_SYNCED && retryCount < RETRIES) {
    Serial.printf("TimeHandler - Not synchronized yet. Retrying %d/%d ...\n", retryCount + 1, RETRIES);
    delay(1000);  // Wait 1000ms before retrying
    retryCount++;
  }

  if (retryCount >= RETRIES) {
    Serial.println("TimeHandler - Failed to synchronize time with NTP server. Proceeding without it.");
    return false;
  } else {
    Serial.println("TimeHandler - Time synchronized successfully.");
    printLocalTime();
    return true;
  }
}