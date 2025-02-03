#include <Arduino.h>

// Define motor control pins
#define MOTOR_IN1 6   // AIN1 pin
#define MOTOR_IN2 7   // AIN2 pin
#define MOTOR_STBY 5  // STBY pin to control driver enable

void setup() {
  // Set motor control pins as outputs
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  pinMode(MOTOR_STBY, OUTPUT);  // Set STBY pin as output

  // Enable motor driver by setting STBY HIGH
  digitalWrite(MOTOR_STBY, HIGH);

  Serial.begin(115200);

  delay(1000);
}

void startMotor(bool reverse) {
  // Start the motor by setting the direction pins

  if (reverse) {
    digitalWrite(MOTOR_IN1, LOW);
    digitalWrite(MOTOR_IN2, HIGH);
    Serial.println("Motor started (reverse)");
  } else {
    digitalWrite(MOTOR_IN1, HIGH);
    digitalWrite(MOTOR_IN2, LOW);
    Serial.println("Motor started");
  }
}

void stopMotor() {
  // Stop the motor by making both pins LOW
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);
  Serial.println("Motor stopped");
}


void loop() {
  // Start motor for 5 seconds
  startMotor(false);
  delay(5000);

  startMotor(true);
  delay(5000);

  // Stop motor for 5 seconds
  stopMotor();
  delay(5000);
}
