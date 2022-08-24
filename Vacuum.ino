#include "Vacuum.h"
#include <AFMotor.h>

Vacuum vacuum;
bool isDirty;
int trips, lastCleanTrip;

void setup() {
}

void loop() {
  lastCleanTrip = 0;

  do {
    isDirty = vacuum.senseTrash();

    if(isDirty) {
      vacuum.cleanTrash();
      lastCleanTrip = trips + 1;
    } else {
      vacuum.move();
    }

    trips = vacuum.getTrips();
  } while(trips - lastCleanTrip < 2);

  vacuum.reset();
}
