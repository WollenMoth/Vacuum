#ifndef Vacuum_h

#define Vacuum_h

class Vacuum {
  public:
    Vacuum();
    void move(int direction);
    void turn(bool side);
    void stop();
    void turnSensor();
    void cleanTrash();
    bool senseTrash();
    int senseObstacles();
};

#endif
