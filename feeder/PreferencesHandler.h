#ifndef PREFERENCES_HANDLER_H
#define PREFERENCES_HANDLER_H
#include <vector>
#include <Arduino.h>

class PreferencesHandler {
public:
  static String getBotToken();
  static String getGroupId();
  static std::vector<String> getFeedingSchedule();
  static String getFeedingScheduleString();
  static int getFeedingWeightPerPortion();
  static int getFeedingBowlWeight();
  static time_t getLastFeedingTime();

  static bool saveBotToken(const String& botToken);
  static bool saveGroupId(const String& groupId);
  static bool saveFeedingSchedule(const String& feedingSchedule);
  static bool saveFeedingWeightPerPortion(const String& weightPerPortion);
  static bool saveFeedingBowlWeight(const String& bowlWeight);
  static void saveLastFeedingTime(const time_t feedingTime);
};

#endif
