#ifndef VOLTAGE_SENSOR_H
#define VOLTAGE_SENSOR_H

#include <Arduino.h>

class VoltageSensor {
public:
  static float readVoltage(int pin, float multiplier);
  static int getBatteryPercentage(float batteryVoltage);
  static String getVoltageInfoMessage();
};

#endif
