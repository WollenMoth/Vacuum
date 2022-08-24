#include "Vacuum.h"
#include <Arduino.h>

#define LEFT false
#define RIGHT true

#define PIN_TRASH_SENSOR 2

#define IDLE_DELAY 10000
#define CLEAN_TIME 2000
#define MAX_MOVE_TIME 3000
#define STOPS 5
#define CLEAN_DELAY MAX_MOVE_TIME / STOPS

Vacuum::Vacuum(): _left_motor(1), _right_motor(2) {
  pinMode(PIN_TRASH_SENSOR, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  _left_motor.setSpeed(255);
  _right_motor.setSpeed(255);

  _direction = FORWARD;

  _last_clean = millis() - CLEAN_DELAY;
  _start_time = 0;
  _run_time = 0;
  _trips = 0;
}

void Vacuum::_startEngine(bool side) {
  side ? _right_motor.run(_direction) : _left_motor.run(_direction);
}

void Vacuum::_stopEngine(bool side) {
  side ? _right_motor.run(RELEASE) : _left_motor.run(RELEASE);
}

void Vacuum::_changeDirection() {
  _direction = _direction == FORWARD ? BACKWARD : FORWARD;
  _trips++;
}

void Vacuum::move() {
  unsigned long move_time = millis() - _start_time + _run_time;

  _startEngine(LEFT);
  _startEngine(RIGHT);

  if(_start_time != 0 && move_time >= MAX_MOVE_TIME) {
    _changeDirection();
    _run_time = 0;
    _start_time = 0;
  }

  if(_start_time == 0)
    _start_time = millis();
}

void Vacuum::stop() {
  _stopEngine(LEFT);
  _stopEngine(RIGHT);
  _run_time += millis() - _start_time;
  _start_time = 0;
}

void Vacuum::cleanTrash() {
  stop();

  digitalWrite(LED_BUILTIN, HIGH);
  delay(CLEAN_TIME);
  digitalWrite(LED_BUILTIN, LOW);

  _last_clean = millis();
}

bool Vacuum::senseTrash() {
  bool isReady = millis() - _last_clean > CLEAN_DELAY;

  return isReady ? !digitalRead(PIN_TRASH_SENSOR) : false;
}

int Vacuum::getTrips() {
  return _trips;
}

void Vacuum::reset() {
  stop();

  _trips = 0;
  _run_time = 0;

  delay(IDLE_DELAY);
}
