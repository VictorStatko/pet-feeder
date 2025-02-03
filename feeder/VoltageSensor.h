#ifndef VOLTAGE_SENSOR_H
#define VOLTAGE_SENSOR_H

#include <Arduino.h>

class VoltageSensor {
private:
  int pin;

  float readRawVoltage();

public:
  VoltageSensor(int pin);

  float readVoltage();
  int getBatteryPercentage(float batteryVoltage);
  String getVoltageInfoMessage();
};

#endif
