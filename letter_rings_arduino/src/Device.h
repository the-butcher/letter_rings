#ifndef Device_h
#define Device_h

#include <Arduino.h>

#include "Define.h"
#include "Matrices.h"

class Device {
private:
    /**
     * last measured device orientation
     */
    static orientation___e orientation;
    /**
     * current device role
     * when paired one device should be PRI and the other should be SEC
     */
    static device_role___e deviceRole;
    /**
     * the modus that was used previous to the current modus
     * needed to know what modus should be rendered when in ACCEL modus and devices are not paired
     */
    static modus_________e prevModus;
    /**
     * the modus currently active
     */
    static modus_________e currModus;
    /**
     * the dterm currently active
     */
    static dterm_________e currDterm;
    /**
     * milliseconds when device was put into PRI
     */
    static uint64_t lastRolePriAssignmentMillis;

    static bitmaps_______t currBitmaps;
    static ledbar________t currLedbar;

public:

    static uint8_t gamknValues[32];

    /**
     * a label to be shown running through all letters of the matrix
     */
    static String label;
    static String word;

    static void setCurrModus(modus_________e currModus);
    static modus_________e getPrevModus();
    static modus_________e getCurrModus();
    static orientation___e getOrientation();
    static void setOrientation(orientation___e orientation);
    static device_role___e getDeviceRole();
    /**
     * set a new role on the device
     * return true if the role was accepted, false otherwise
     */
    static bool setDeviceRole(device_role___e deviceRole);

    static void setCurrBitmaps(bitmaps_______t currBitmaps);
    static bitmaps_______t getCurrBitmaps();

    static void setCurrLedbar(ledbar________t currLedbar);
    static ledbar________t getCurrLedbar();

    static void setCurrDterm(dterm_________e currDterm);
    static dterm_________e getCurrDterm();

    static bool powerup();
    static bool depower();

};

#endif