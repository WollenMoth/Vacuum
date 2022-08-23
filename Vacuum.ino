#include "Vacuum.h"
#include <AFMotor.h>
#include <AsyncTimer.h>

#define LEFT false
#define RIGHT true

Vacuum vacuum;

void setup() {
  vacuum.turnSensor();
}

void loop() {
  vacuum.move(FORWARD);
  delay(1000);
  vacuum.stop();
  delay(1000);
  vacuum.turn(LEFT);
  delay(1000);
  vacuum.stop();
  delay(1000);
  vacuum.move(BACKWARD);
  delay(1000);
  vacuum.stop();
  delay(1000);
  vacuum.turn(RIGHT);
  delay(1000);
  vacuum.stop();
  delay(1000);
}
