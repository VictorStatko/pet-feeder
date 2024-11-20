#ifndef TELEGRAM_HANDLER_H
#define TELEGRAM_HANDLER_H

#include <Arduino.h>

class TelegramHandler {
public:
  static void sendBotMessage(const String& message);
};

#endif
