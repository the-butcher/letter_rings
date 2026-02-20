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
    static String label;  // a label to be shown running through all letters of the matrix, depending on hand orienation
    static String word;
    static bool powerup();
    static bool depower();
    static orientation___e getOrientation();
    static void setOrientation(orientation___e orientation);
};

#endif