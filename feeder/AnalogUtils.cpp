#include "AnalogUtils.h"

const float ADC_MULTIPLIER = 0.9434;

int AnalogUtils::readValue(int pin) {

  pinMode(pin, INPUT);

  analogReadResolution(12);

  int adcValue = analogRead(pin);

  return (int)adcValue * ADC_MULTIPLIER;
}