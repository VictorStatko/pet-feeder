#ifndef PREFERENCES_HANDLER_H
#define PREFERENCES_HANDLER_H

#include <Arduino.h>

class PreferencesHandler {
public:
  static String getBotToken();
  static String getGroupId();
  static void saveBotToken(const String& botToken);
  static void saveGroupId(const String& groupId);
};

#endif
