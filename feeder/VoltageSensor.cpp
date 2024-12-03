#include "VoltageSensor.h"
#include "AnalogUtils.h"
#include "Messages.h"

VoltageSensor::VoltageSensor(int pin, float multiplier)
  : pin(pin), multiplier(multiplier) {}

float VoltageSensor::readRawVoltage() {
  const int attempts = 10;                  // Number of attempts
  const int readingsPerAttempt = 5;         // Number of readings per attempt
  const int delayBetweenAttemptsMs = 1000;  // Delay between attempts in milliseconds

  float totalVoltage = 0;

  for (int attempt = 0; attempt < attempts; ++attempt) {
    float attemptVoltage = 0;

    // Perform multiple readings for accuracy during this attempt
    for (int reading = 0; reading < readingsPerAttempt; ++reading) {
      int adcValue = AnalogUtils::readValue(pin);
      float adcVoltage = ((float)adcValue / 4095.0) * 3.3;
      attemptVoltage += adcVoltage * 5.0;
    }

    // Average the readings for this attempt
    attemptVoltage /= readingsPerAttempt;
    totalVoltage += attemptVoltage;

    delay(delayBetweenAttemptsMs);
  }

  // Return the average voltage across all attempts
  return totalVoltage / attempts;
}

float VoltageSensor::readVoltage() {
  return readRawVoltage() * multiplier;
}

int VoltageSensor::getBatteryPercentage() {
  float batteryVoltage = readVoltage();

  if (batteryVoltage >= 4.40) return 0;
  else if (batteryVoltage >= 4.20) return 100;
  else if (batteryVoltage >= 4.12) return 95;
  else if (batteryVoltage >= 4.05) return 90;
  else if (batteryVoltage >= 4.00) return 85;
  else if (batteryVoltage >= 3.95) return 80;
  else if (batteryVoltage >= 3.90) return 75;
  else if (batteryVoltage >= 3.85) return 70;
  else if (batteryVoltage >= 3.82) return 65;
  else if (batteryVoltage >= 3.78) return 60;
  else if (batteryVoltage >= 3.74) return 55;
  else if (batteryVoltage >= 3.70) return 50;
  else if (batteryVoltage >= 3.65) return 45;
  else if (batteryVoltage >= 3.60) return 40;
  else if (batteryVoltage >= 3.55) return 35;
  else if (batteryVoltage >= 3.50) return 30;
  else if (batteryVoltage >= 3.45) return 25;
  else if (batteryVoltage >= 3.40) return 20;
  else if (batteryVoltage >= 3.20) return 15;
  else if (batteryVoltage >= 3.00) return 10;
  else if (batteryVoltage >= 2.50) return 5;
  else return 0;
}

String VoltageSensor::getVoltageInfoMessage(const String& partName) {
  float voltage = readVoltage();
  int percentage = getBatteryPercentage();

  String percentageStr = (percentage != 0) ? String(percentage) + "%" : MESSAGE_UNKNOWN;
  return partName + " " + percentageStr + " (" + String(voltage) + " V)";
}
