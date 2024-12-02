#ifndef VOLTAGE_SENSOR_H
#define VOLTAGE_SENSOR_H

#include <Arduino.h>

class VoltageSensor {
private:
  float multiplier;
  int pin;

  float readRawVoltage();

public:
  VoltageSensor(int pin, float multiplier);

  float readVoltage();
  int getBatteryPercentage();
  String getVoltageInfoMessage(const String& partName);
};

#endif
