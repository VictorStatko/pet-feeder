#ifndef MESSAGES_H
#define MESSAGES_H

#include <Arduino.h>

const String MESSAGE_READY_TO_USE = "Устройство готово к использованию.";
const String MESSAGE_RTC_MODULE_ERROR = "Возникла ошибка при чтении времени из модуля часов. Просто игнорируйте это сообщение при первом запуске устройства. При повторном появлении - замените батарейку.";
const String MESSAGE_TIME_SYNC_ERROR = "Возникла ошибка при синхронизации времени. Текущее время (UTC) - ";
const String MESSAGE_TIME_RETRIEVING_ERROR = "Возникла ошибка при получении времени из модуля часов - проверьте подключение. Текущее время (UTC) - ";
const String MESSAGE_TIME_TO_FEED = "Настало время кормления.";
const String MESSAGE_BATTERY_PERCENTAGE = "Заряд батареи";
const String MESSAGE_UNKNOWN = "unknown";
const String MESSAGE_END_SEPARATOR = "\n----------------------------------\n";
const String MESSAGE_WEIGHT_ERROR = "Возникла ошибка при получении значения текущего веса (проверьте подключение) - ";
const String MESSAGE_FEEDING_MISSED = "кормление пропущено.";
const String MESSAGE_FEEDING_STOPPED = "кормление остановлено.";
const String MESSAGE_FEEDING_NO_WEIGHT_CHANGE = "Закончился корм, либо заблокирован мотор. Кормление остановлено.";
const String MESSAGE_FEEDING_SUCCESS = "Кормление успешно завершено.";
const String MESSAGE_FEEDING_ENOUGH_FOOD = "Еды достаточно - " + MESSAGE_FEEDING_MISSED;
const String MESSAGE_FEEDING_START = "Кормление запущено - необходимо добавить ";
const String MESSAGE_GRAMM = "гр";
const String MESSAGE_NO_BOWL = "Отсутствует миска - " + MESSAGE_FEEDING_MISSED;
const String MESSAGE_SETTINGS_INVALID_BOT_TOKEN = "Токен телеграм бота не задан или не валиден. Выполните конфигурацию заново.";
const String MESSAGE_SETTINGS_INVALID_BOT_GROUP_ID = "Group ID телеграм бота не задан или не валиден. Выполните конфигурацию заново.";
const String MESSAGE_SETTINGS_INVALID_SCHEDULING = "Расписание кормления не задано или не валидно. Выполните конфигурацию заново.";
const String MESSAGE_SETTINGS_INVALID_PORTION_WEIGHT = "Вес порции не задан или не валиден. Выполните конфигурацию заново.";
const String MESSAGE_SETTINGS_INVALID_BOWL_WEIGHT = "Вес миски не задан или не валиден. Выполните конфигурацию заново.";
const String MESSAGE_CURRENT_SETTINGS = "Текущие настройки.";
const String MESSAGE_CURRENT_SETTINGS_SCHEDULING = "Расписание кормления (UTC таймзона): ";
const String MESSAGE_CURRENT_SETTINGS_PORTION_WEIGHT = "Вес порции (гр): ";
const String MESSAGE_CURRENT_SETTINGS_BOWL_WEIGHT = "Вес миски (гр): ";

#endif
