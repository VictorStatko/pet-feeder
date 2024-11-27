#ifndef MESSAGES_H
#define MESSAGES_H

#include <Arduino.h>

const String MESSAGE_READY_TO_USE = "Устройство готово к использованию.";
const String MESSAGE_TIME_SYNC_ERROR = "Возникла ошибка при синхронизации времени. Переподключение через 5 минут.";
const String MESSAGE_TIME_TO_FEED = "Настало время кормить питомца.";
const String MESSAGE_BATTERY_PERCENTAGE = "Заряд батареи";
const String MESSAGE_UNKNOWN = "unknown";
const String MESSAGE_END_SEPARATOR = "\n----------------------------------\n";

#endif
