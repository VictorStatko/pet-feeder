#ifndef PREFERENCES_HANDLER_H
#define PREFERENCES_HANDLER_H
#include <vector>
#include <Arduino.h>

class PreferencesHandler {
public:
  static String getBotToken();
  static String getGroupId();
  static std::vector<String> getFeedingSchedule();
  static int getFeedingWeightPerPortion();
  static int getFeedingBowlWeight();

  static void saveBotToken(const String& botToken);
  static void saveGroupId(const String& groupId);
  static bool saveFeedingSchedule(const String& feedingSchedule);
  static bool saveFeedingWeightPerPortion(const String& weightPerPortion);
  static bool saveFeedingBowlWeight(const String& bowlWeight);
};

#endif
