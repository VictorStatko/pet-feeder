#ifndef WEIGHT_SENSOR_H
#define WEIGHT_SENSOR_H

#include <Arduino.h>
#include "HX711.h"

class WeightSensor {
private:
  int dtPin;
  int sckPin;
  int scale;
  int offset;
  HX711 sensor;

public:
  WeightSensor(int dtPin, int sckPin, int scale, int offset);

  float readWeight();

  void begin();

  void end();
};

#endif
