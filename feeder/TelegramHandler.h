#ifndef TELEGRAM_HANDLER_H
#define TELEGRAM_HANDLER_H

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

class TelegramHandler {
public:
  TelegramHandler();
  void begin(const String& botToken, const String& groupId);
  void sendBotMessage(const String& message);

private:
  String botToken;
  String groupId;
  WiFiClientSecure securedClient;
  UniversalTelegramBot* bot;
};

#endif
