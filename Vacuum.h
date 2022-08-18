#ifndef Vacuum_h

#define Vacuum_h

class Vacuum {
  private:
    void _startEngine(bool side, bool direction);
  public:
    Vacuum();
    void move(bool direction);
    void turn(bool side);
    void stop();
    void cleanTrash();
    bool senseTrash();
    int senseObstacles();
};

#endif
