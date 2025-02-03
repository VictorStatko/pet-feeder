#ifndef DC_MOTOR_H
#define DC_MOTOR_H

#include <Arduino.h>

class DCMotor {

private:
  int pinIn1;
  int pinIn2;
  int pinStby;

public:
  DCMotor(int pinIn1, int pinIn2, int pinStby);

  void begin();

  void end();

  void startMotor(bool reverse);

  void stopMotor();
};

#endif
