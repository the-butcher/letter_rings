#include <Device.h>

// Adafruit_8x8matrix Matrix::baseMatrix();
modus_________e Device::prevModus = MODUS________WORDS;
modus_________e Device::currModus = MODUS________ACCEL;

orientation___e Device::orientation = ORIENTATION______UP;

String Device::label = "LABEL";
String Device::word = "WORD";
bitmaps_______t Device::currBitmaps = {{
                                           BITMAP_PAC_____CLOSE,  // this deletes the shape of pacman ahead of its path
                                           0,                     //
                                           LED_OFF                //
                                       },
                                       {
                                           BITMAP_PAC____OPEN_R,  // this draws the shape of pacman
                                           0,                     //
                                           LED_ON                 //
                                       }};

device_role___e Device::deviceRole = DEVICE_ROLE_____ANY;

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
    Device::orientation = orientation;
    // this should only be called when orientation actually changes
    Matrices::matrixA.setOrientation(orientation);
    Matrices::matrixB.setOrientation(orientation);
    Matrices::matrixC.setOrientation(orientation);
    Matrices::matrixD.setOrientation(orientation);
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

void Device::setDeviceRole(device_role___e deviceRole) {
    Device::deviceRole = deviceRole;
    // if (Device::deviceRole == DEVICE_ROLE_____PRI) {
    //     Device::deviceRoleMessagePending = true;
    // }
}
