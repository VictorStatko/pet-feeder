#include "WiFiManagerWrapper.h"
#include "PreferencesHandler.h"
#include <WiFiManager.h>

const char* const DEFAULT_AP_NAME = "PetFeeder";
const char* const DEFAULT_AP_PASSWORD = "11111111";
const char* const TELEGRAM_BOT_TOKEN_KEY = "botToken";
const char* const TELEGRAM_GROUP_ID_KEY = "groupId";
const int CONFIG_PORTAL_TIMEOUT_S = 300;

void WiFiManagerWrapper::setupWiFiManager() {
  Serial.println("WiFiManagerWrapper - Setting up WiFiManager");

  WiFiManager wm;
  wm.setConfigPortalTimeout(CONFIG_PORTAL_TIMEOUT_S);

  // Retrieve stored preferences for bot token and group ID
  Serial.println("WiFiManagerWrapper - Retrieving stored preferences for bot token and group ID");
  String botToken = PreferencesHandler::getBotToken();
  String groupId = PreferencesHandler::getGroupId();

  Serial.println("WiFiManagerWrapper - Retrieved bot token: " + botToken);
  Serial.println("WiFiManagerWrapper - Retrieved group ID: " + groupId);

  // Add custom parameters for WiFiManager
  Serial.println("WiFiManagerWrapper - Adding custom parameters to WiFiManager");
  WiFiManagerParameter custom_bot_token(TELEGRAM_BOT_TOKEN_KEY, "Telegram Bot Token", botToken.c_str(), 64);
  WiFiManagerParameter custom_group_id(TELEGRAM_GROUP_ID_KEY, "Telegram Group ID", groupId.c_str(), 20);

  wm.addParameter(&custom_bot_token);
  wm.addParameter(&custom_group_id);

  // Start WiFi configuration portal
  Serial.println("WiFiManagerWrapper - Starting configuration portal");
  if (!wm.startConfigPortal(DEFAULT_AP_NAME, DEFAULT_AP_PASSWORD)) {
    Serial.println("WiFiManagerWrapper - Failed to connect or configure. Continuing with existing settings.");
  } else {
    Serial.println("WiFiManagerWrapper - WiFi configuration completed successfully.");
  }

  // Save updated preferences
  Serial.println("WiFiManagerWrapper - Saving updated preferences");
  PreferencesHandler::saveBotToken(custom_bot_token.getValue());
  PreferencesHandler::saveGroupId(custom_group_id.getValue());
  Serial.println("WiFiManagerWrapper - Preferences saved");
}

void WiFiManagerWrapper::autoConnectWiFi() {
  Serial.println("WiFiManagerWrapper - Attempting to auto-connect to WiFi");
  WiFiManager wm;
  wm.setConfigPortalTimeout(1);

  if (!wm.autoConnect()) {
    Serial.println("WiFiManagerWrapper - Auto-connect failed. Check WiFi settings.");
  } else {
    Serial.println("WiFiManagerWrapper - Auto-connect successful.");
  }
}
