#include "WeightSensor.h"
#include "UsedPins.h"
#include "Messages.h"
#include <cmath>

WeightSensor::WeightSensor(int dtPin, int sckPin, int scale, int offset)
  : dtPin(dtPin), sckPin(sckPin), scale(scale), offset(offset) {
}

void WeightSensor::begin() {
  Serial.println("WeightSensor - Initializing sensor...");
  sensor.begin(dtPin, sckPin);
  Serial.println("WeightSensor - Setting scale...");
  sensor.set_scale(scale);
  Serial.println("WeightSensor - Setting offset...");
  sensor.set_offset(offset);
  Serial.println("WeightSensor - Powering up sensor...");
  sensor.power_up();
  Serial.println("WeightSensor - Sensor initialized");
}

void WeightSensor::end() {
  Serial.println("WeightSensor - Powering down sensor...");
  sensor.power_down();
  Serial.println("WeightSensor - Sensor powered down");
}

float WeightSensor::readWeight() {
  Serial.println("WeightSensor - Reading weight...");
  if (sensor.wait_ready_timeout(1000)) {
    float weight = sensor.get_units(10);  // Get an average of 10 readings
    Serial.print("WeightSensor - Weight read: ");
    Serial.println(weight);
    return weight;
  } else {
    Serial.println("WeightSensor - Sensor not ready, returning NAN");
    return NAN;
  }
}
