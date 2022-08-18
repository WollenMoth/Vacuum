#include "Arduino.h"
#include "Vacuum.h"

#define LEFT false
#define RIGHT true
#define BACKWARD false
#define FORWARD true

Vacuum::Vacuum() {
}

void _startEngine(bool side, bool direction) {
  switch(side) {
    case LEFT:
      break;
    case RIGHT:
      break;
  }
}

void _stopEngine(bool side) {
  switch(side) {
    case LEFT:
      break;
    case RIGHT:
      break;
  }
}

void Vacuum::move(bool direction) {
  _startEngine(LEFT, direction);
  _startEngine(RIGHT, direction);
}

void Vacuum::turn(bool side) {
  switch (side) {
    case LEFT:
      _startEngine(LEFT, BACKWARD);
      _startEngine(RIGHT, FORWARD);
      break;
    case RIGHT:
      _startEngine(LEFT, FORWARD);
      _startEngine(RIGHT, BACKWARD);
      break;
  }
}

void Vacuum::stop() {
  _stopEngine(LEFT);
  _stopEngine(RIGHT);
}

void Vacuum::cleanTrash() {
}

bool Vacuum::senseTrash() {
  return false;
}

int Vacuum::senseObstacles() {
  return 0;
}
