#ifndef WIFI_MANAGER_WRAPPER_H
#define WIFI_MANAGER_WRAPPER_H

#include <Arduino.h>
#include "TelegramHandler.h"

class WiFiManagerWrapper {
public:
  static void setupWiFiManager(TelegramHandler& telegramHandler);
  static void autoConnectWiFi();
};

#endif
