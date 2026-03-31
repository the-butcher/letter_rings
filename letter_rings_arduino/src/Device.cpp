#include <Device.h>

// Adafruit_8x8matrix Matrix::baseMatrix();
modus_________e Device::prevModus = MODUS________CHARS;
modus_________e Device::currModus = MODUS________CHARS;
dterm_________e Device::currDterm = DTERM_________NONE;

orientation___e Device::orientation = ORIENTATION______UP;

String Device::label = "LABEL";
String Device::word = "WORD";
bitmaps_______t Device::currBitmaps = { {
                                           BITMAP_PAC_____CLOSE,  // this deletes the shape of pacman ahead and behind its path
                                           0
                                       },
                                       {
                                           BITMAP_PAC____OPEN_R,  // this draws the shape of pacman
                                           0
                                       },
                                       {
                                           BITMAP_________EMPTY,  // reserve
                                           0
                                       },
                                       {
                                           BITMAP_________EMPTY,  // reserve
                                           0
                                       } };

uint8_t Device::gamknValues[32] = {};

ledbar________t Device::currLedbar = {
    0,
    0
}; // empty ledbar

device_role___e Device::deviceRole = DEVICE_ROLE_____ANY;
uint64_t Device::lastRolePriAssignmentMillis = 0;

bool Device::powerup() {
    // https://www.desmos.com/calculator/e8oafd81vd?lang=de
    for (uint8_t i = 0; i < 32; i++) {
        Device::gamknValues[i] = round(pow((cos(i * PI / 32) + 1) * 0.5, 12) * 12);
        Serial.printf("kn value    %03d -> %03d\n", i, Device::gamknValues[i]);
    }
    return true;
}

bool Device::depower() {
    // nothing
    return true;
}

void Device::setCurrBitmaps(bitmaps_______t currBitmaps) {
    Device::currBitmaps = currBitmaps;
    Device::setCurrDterm(DTERM________GAMPM);
}

bitmaps_______t Device::getCurrBitmaps() {
    return Device::currBitmaps;
}

void Device::setCurrLedbar(ledbar________t currLedbar) {
    Device::currLedbar = currLedbar;
    Device::setCurrDterm(DTERM________GAMKN);
}

ledbar________t Device::getCurrLedbar() {
    return Device::currLedbar;
}

void Device::setCurrDterm(dterm_________e currDterm) {
    if (currDterm != Device::currDterm) {
        if (Device::currDterm == DTERM________GAMKN) {
            Matrices::setBrightness(Matrices::getBrightness(), true);
        }
        Device::currDterm = currDterm;
    }
}

dterm_________e Device::getCurrDterm() {
    return Device::currDterm;
}

orientation___e Device::getOrientation() {
    return Device::orientation;
}

void Device::setOrientation(orientation___e orientation) {
    Device::orientation = orientation;  // remember
    // this should only be called when orientation actually changes
    Matrices::setOrientation(orientation);  // apply
}

void Device::setCurrModus(modus_________e currModus) {
    if (Device::currModus != MODUS________ACCEL) {
        Device::prevModus = Device::currModus;
        Device::setCurrDterm(DTERM_________NONE); // be sure to reset DTERM, this will also force a brightness update if the device was actually in GAMKN mode
    }
    Device::currModus = currModus;
}
modus_________e Device::getPrevModus() {
    return Device::prevModus;
}
modus_________e Device::getCurrModus() {
    return Device::currModus;
}

device_role___e Device::getDeviceRole() {
    return Device::deviceRole;
}

bool Device::setDeviceRole(device_role___e deviceRole) {
    if (deviceRole == DEVICE_ROLE_____PRI) {  // always accept PRI
        Device::deviceRole = deviceRole;
        Device::lastRolePriAssignmentMillis = millis();
        return true;
    } else if (deviceRole == DEVICE_ROLE_____ANY) {
        if (Device::deviceRole == DEVICE_ROLE_____PRI) {  // when in PRI only accept ANY after a while
            if ((millis() - Device::lastRolePriAssignmentMillis) > ROLE_PRI____DURATION_MS || (currDterm == DTERM________GAMPM && ACCEPT__ROLE_DOWNGRADE)) { // ACCEPT__ROLE_DOWNGRADE is true for R, false for L
                Device::deviceRole = deviceRole;
                return true;
            } else {
                return false;
            }
        } else {  // when in ANY or SEC or UNKNOWN always accept ANY
            Device::deviceRole = deviceRole;
            return true;
        }
    } else if (deviceRole == DEVICE_ROLE_____SEC) {
        if (Device::getDeviceRole() == DEVICE_ROLE_____PRI && !ACCEPT__ROLE_DOWNGRADE) {  // when in PRI never accept SEC, in rare cases both devices go to PRI simultaneously, and they would also turn the other respective device into SEC simultaneously
            return false;
        } else {
            Device::deviceRole = deviceRole;
            return true;
        }
    } else {  // unknown role, dont accept
        return false;
    }
}
