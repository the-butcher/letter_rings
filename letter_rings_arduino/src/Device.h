#ifndef Device_h
#define Device_h

#include <Arduino.h>

#include "Define.h"
#include "Matrices.h"

class Device {
   private:
    static orientation___e orientation;
    static device_role___e deviceRole;
    // static bool deviceRoleMessagePending;
    static modus_________e prevModus;
    static modus_________e currModus;

   public:
    static String label;  // a label to be shown running through all letters of the matrix, depending on hand orienation
    static String word;
    static bitmaps_______t currBitmaps;
    // static bitmaps_______t voidBitmaps;
    static bool powerup();
    static bool depower();
    static void setCurrModus(modus_________e currModus);
    static modus_________e getPrevModus();
    static modus_________e getCurrModus();
    static orientation___e getOrientation();
    static void setOrientation(orientation___e orientation);
    static device_role___e getDeviceRole();
    static void setDeviceRole(device_role___e deviceRole);
    static bitmaps_______t getSendBitmaps();
};

#endif