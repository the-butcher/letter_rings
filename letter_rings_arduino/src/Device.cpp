#include <Device.h>

// Adafruit_8x8matrix Matrix::baseMatrix();
modus_________e Device::modus = MODUS________LABEL;
orientation___e Device::orientation = ORIENTATION______UP;
String Device::label = "LABEL";
String Device::word = "WORD";

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
