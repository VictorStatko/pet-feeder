#ifndef WIFI_MANAGER_WRAPPER_H
#define WIFI_MANAGER_WRAPPER_H

#include <Arduino.h>

class WiFiManagerWrapper {
public:
  static void setupWiFiManager();
  static void autoConnectWiFi();
};

#endif
