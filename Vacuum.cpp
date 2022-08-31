#include "Vacuum.h"
#include <Arduino.h>

#define LEFT false
#define RIGHT true

#define PIN_TRASH_SENSOR 2

#define MOTOR_SPEED 100

#define IDLE_DELAY 10000
#define CLEAN_TIME 2000
#define MAX_MOVE_TIME 2000

Vacuum::Vacuum() : _left_motor(1), _right_motor(2)
{
  pinMode(PIN_TRASH_SENSOR, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  _left_motor.setSpeed(MOTOR_SPEED);
  _right_motor.setSpeed(MOTOR_SPEED);

  _direction = FORWARD;

  _start_time = 0;
  _run_time = 0;
  _trips = 0;
}

void Vacuum::_startEngine(bool side)
{
  side ? _right_motor.run(_direction) : _left_motor.run(_direction);
}

void Vacuum::_stopEngine(bool side)
{
  side ? _right_motor.run(RELEASE) : _left_motor.run(RELEASE);
}

void Vacuum::_changeDirection()
{
  _direction = _direction == FORWARD ? BACKWARD : FORWARD;
  _trips++;
}

void Vacuum::move()
{
  unsigned long move_time = millis() - _start_time + _run_time;

  _startEngine(LEFT);
  _startEngine(RIGHT);

  if (_start_time != 0 && move_time >= MAX_MOVE_TIME)
  {
    _changeDirection();
    _run_time = 0;
    _start_time = 0;
  }

  if (_start_time == 0)
    _start_time = millis();
}

void Vacuum::stop()
{
  _stopEngine(LEFT);
  _stopEngine(RIGHT);

  if (_start_time != 0)
  {
    _run_time = millis() - _start_time;
    _start_time = 0;
  }
}

void Vacuum::cleanTrash()
{
  stop();

  digitalWrite(LED_BUILTIN, HIGH);
  delay(CLEAN_TIME);
  digitalWrite(LED_BUILTIN, LOW);
}

bool Vacuum::senseTrash()
{
  return digitalRead(PIN_TRASH_SENSOR);
}

int Vacuum::getTrips()
{
  return _trips;
}

void Vacuum::reset()
{
  stop();

  _trips = 0;
  _run_time = 0;

  delay(IDLE_DELAY);
}
