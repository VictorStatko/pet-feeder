#include "DCMotor.h"

DCMotor::DCMotor(int pinIn1, int pinIn2, int pinStby)
  : pinIn1(pinIn1), pinIn2(pinIn2), pinStby(pinStby) {
  Serial.println("DCMotor - Constructor initialized");
}

void DCMotor::begin() {
  Serial.println("DCMotor - Initializing pins and enabling motor standby mode");
  pinMode(pinIn1, OUTPUT);
  pinMode(pinIn2, OUTPUT);
  pinMode(pinStby, OUTPUT);

  digitalWrite(pinStby, HIGH);
  delay(1000);
  Serial.println("DCMotor - Initialization complete");
}

void DCMotor::end() {
  Serial.println("DCMotor - Stopping motor and disabling standby");
  digitalWrite(pinIn1, LOW);
  digitalWrite(pinIn2, LOW);
  digitalWrite(pinStby, LOW);
  Serial.println("DCMotor - Motor and standby disabled");
}

void DCMotor::startMotor(bool reverse) {
  Serial.println("DCMotor - Starting motor (IN1: HIGH, IN2: LOW)");
  if (reverse) {
    digitalWrite(pinIn1, LOW);
    digitalWrite(pinIn2, HIGH);
    Serial.println("DCMotor - Motor started (reverse)");
  } else {
    digitalWrite(pinIn1, HIGH);
    digitalWrite(pinIn2, LOW);
    Serial.println("DCMotor - Motor started");
  }
}

void DCMotor::stopMotor() {
  Serial.println("DCMotor - Stopping motor (IN1: LOW, IN2: LOW)");
  digitalWrite(pinIn1, LOW);
  digitalWrite(pinIn2, LOW);
  Serial.println("DCMotor - Motor stopped");
}
