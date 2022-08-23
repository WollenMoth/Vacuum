#include "Vacuum.h"
#include <Arduino.h>
#include <AFMotor.h>
#include <Servo.h>
#include <AsyncTimer.h>

#define LEFT false
#define RIGHT true

AF_DCMotor left_motor(1);
AF_DCMotor right_motor(2);
Servo servo;
AsyncTimer t;

Vacuum::Vacuum() {
  left_motor.setSpeed(255);
  right_motor.setSpeed(255);
  servo.attach(9);
}

void _startEngine(bool side, int direction) {
  side ? right_motor.run(direction) : left_motor.run(direction);
}

void _stopEngine(bool side) {
  side ? right_motor.run(RELEASE) : left_motor.run(RELEASE);
}

void Vacuum::move(int direction) {
  _startEngine(LEFT, direction);
  _startEngine(RIGHT, direction);
}

void Vacuum::turn(bool side) {
  if(side) {
    _startEngine(LEFT, BACKWARD);
    _startEngine(RIGHT, FORWARD);
  } else {
    _startEngine(LEFT, BACKWARD);
    _startEngine(RIGHT, FORWARD);
  }
}

void Vacuum::stop() {
  _stopEngine(LEFT);
  _stopEngine(RIGHT);
}

void Vacuum::turnSensor() {
  t.setInterval([]() {
    servo.write(0);
    t.setTimeout([]() {
      servo.write(180);
    }, 1000);
  }, 2000);
}

void Vacuum::cleanTrash() {
}

bool Vacuum::senseTrash() {
  return false;
}

int Vacuum::senseObstacles() {
  return 0;
}
