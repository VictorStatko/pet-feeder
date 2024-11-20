#include "PreferencesHandler.h"
#include <Preferences.h>

const char* const PREF_TELEGRAM = "telegram";
const char* const PREF_TELEGRAM_BOT_TOKEN_KEY = "botToken";
const char* const PREF_TELEGRAM_GROUP_ID_KEY = "groupId";
const char* const DEFAULT_TELEGRAM_BOT_TOKEN = "";
const char* const DEFAULT_TELEGRAM_GROUP_ID = "";

String PreferencesHandler::getBotToken() {
  Serial.println("PreferencesHandler - Reading bot token from preferences");
  Preferences preferences;
  preferences.begin(PREF_TELEGRAM, true);
  String botToken = preferences.getString(PREF_TELEGRAM_BOT_TOKEN_KEY, DEFAULT_TELEGRAM_BOT_TOKEN);
  preferences.end();
  Serial.println("PreferencesHandler - Retrieved bot token: " + botToken);
  return botToken;
}

String PreferencesHandler::getGroupId() {
  Serial.println("PreferencesHandler - Reading group ID from preferences");
  Preferences preferences;
  preferences.begin(PREF_TELEGRAM, true);
  String groupId = preferences.getString(PREF_TELEGRAM_GROUP_ID_KEY, DEFAULT_TELEGRAM_GROUP_ID);
  preferences.end();
  Serial.println("PreferencesHandler - Retrieved group ID: " + groupId);
  return groupId;
}

void PreferencesHandler::saveBotToken(const String& botToken) {
  Serial.println("PreferencesHandler - Saving bot token to preferences: " + botToken);
  Preferences preferences;
  preferences.begin(PREF_TELEGRAM, false);
  preferences.putString(PREF_TELEGRAM_BOT_TOKEN_KEY, botToken);
  preferences.end();
  Serial.println("PreferencesHandler - Bot token saved successfully");
}

void PreferencesHandler::saveGroupId(const String& groupId) {
  Serial.println("PreferencesHandler - Saving group ID to preferences: " + groupId);
  Preferences preferences;
  preferences.begin(PREF_TELEGRAM, false);
  preferences.putString(PREF_TELEGRAM_GROUP_ID_KEY, groupId);
  preferences.end();
  Serial.println("PreferencesHandler - Group ID saved successfully");
}
