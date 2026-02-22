#include <Device.h>

// Adafruit_8x8matrix Matrix::baseMatrix();
modus_________e Device::prevModus = MODUS________LABEL;
modus_________e Device::currModus = MODUS________LABEL;

orientation___e Device::orientation = ORIENTATION______UP;

String Device::label = "LABEL";
String Device::word = "WORD";
bitmaps_______t Device::currBitmaps = {{
                                           BITMAP_PAC____OPEN_R,  //
                                           0                      //
                                       },
                                       {
                                           BITMAP_PAC____OPEN_R,  // TODO :: trailing ghost
                                           0                      //
                                       }};

device_role___e Device::deviceRole = DEVICE_ROLE_____ANY;
// bool Device::voidBitmapsMessagePending = false;

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

bitmaps_______t Device::getSendBitmaps() {
    return Device::currBitmaps;
    // if (Device::deviceRole == DEVICE_ROLE_____PRI) {
    //     return &Device::currBitmaps;
    // } else if (Device::voidBitmapsMessagePending) {
    //     Device::voidBitmapsMessagePending = false;
    //     // Serial.print(DEVICE____________SIDE);
    //     // Serial.println("returning void bitmaps");
    //     return &Device::voidBitmaps;
    // } else {
    //     return nullptr;
    // }
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
