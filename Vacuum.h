#ifndef Vacuum_h

#define Vacuum_h

#include <AFMotor.h>

class Vacuum {
  private:
    AF_DCMotor _left_motor, _right_motor;
    int _direction, _trips;
    unsigned long _start_time, _run_time;
    unsigned long _last_clean;
    void _startEngine(bool);
    void _stopEngine(bool);
    void _changeDirection();
  public:
    Vacuum();
    void move();
    void stop();
    void cleanTrash();
    bool senseTrash();
    int getTrips();
    void reset();
};

#endif
