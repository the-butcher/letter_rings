#ifndef Device_h
#define Device_h

#include <Arduino.h>

#include "Define.h"
#include "Matrices.h"

class Device {
   private:
    static orientation___e orientation;

   public:
    static modus_________e modus;
    static bool powerup();
    static bool depower();
    static orientation___e getOrientation();
    static void setOrientation(orientation___e orientation);
};

#endif