#ifndef MESSAGES_H
#define MESSAGES_H

#include <Arduino.h>

const String MESSAGE_READY_TO_USE = "Устройство готово к использованию.";
const String MESSAGE_RTC_MODULE_ERROR = "Возникла ошибка при чтении времени из модуля часов. Просто игнорируйте это сообщение при первом запуске устройства. При повторном появлении - замените батарейку.";
const String MESSAGE_TIME_SYNC_ERROR = "Возникла ошибка при синхронизации времени. Текущее время (UTC) - ";
const String MESSAGE_TIME_RETRIEVING_ERROR = "Возникла критическая ошибка при получении времени из модуля часов - проверьте подключение. Текущее время (UTC) - ";
const String MESSAGE_TIME_TO_FEED = "Настало время кормить питомца.";
const String MESSAGE_BATTERY_PERCENTAGE = "Заряд батареи";
const String MESSAGE_UNKNOWN = "unknown";
const String MESSAGE_END_SEPARATOR = "\n----------------------------------\n";

#endif
