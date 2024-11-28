#include "AnalogUtils.h"

int AnalogUtils::readValue(int pin) {

  pinMode(pin, INPUT);

  analogReadResolution(12);

  return analogRead(pin);
}