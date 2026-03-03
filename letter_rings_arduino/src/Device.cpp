#include <Device.h>

// Adafruit_8x8matrix Matrix::baseMatrix();
modus_________e Device::prevModus = MODUS________BREAK;  // MODUS________WORDS;
modus_________e Device::currModus = MODUS________BREAK;  // MODUS________ACCEL;

orientation___e Device::orientation = ORIENTATION______UP;

String Device::label = "LABEL";
String Device::word = "WORD";
bitmaps_______t Device::currBitmaps = { {
                                           BITMAP_PAC_____CLOSE,  // this deletes the shape of pacman ahead of its path
                                           0,                     //
                                           LED_OFF                //
                                       },
                                       {
                                           BITMAP_PAC____OPEN_R,  // this draws the shape of pacman
                                           0,                     //
                                           LED_ON                 //
                                       } };

device_role___e Device::deviceRole = DEVICE_ROLE_____ANY;
uint64_t Device::lastRolePriAssignmentMillis = 0;

bool Device::powerup() {
    // nothing
    return true;
}

bool Device::depower() {
    // nothing
    return true;
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
            if ((millis() - Device::lastRolePriAssignmentMillis) > ROLE_PRI____DURATION_MS) {
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
        if (Device::getDeviceRole() == DEVICE_ROLE_____PRI) {  // when in PRI never accept SEC, in rare cases both devices go to PRI simultaneously, and they would also turn the other respective device into SEC simultaneously
            return false;
        } else {
            Device::deviceRole = deviceRole;
            return true;
        }
    } else {  // unknown role, dont accept
        return false;
    }
}
