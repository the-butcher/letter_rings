#include "Buttons.h"

Button Buttons::buttonA(GPIO_NUM_2);
Button Buttons::buttonB(GPIO_NUM_1);
Button Buttons::buttonC(GPIO_NUM_0);
button_action_e Buttons::buttonAction = BUTTON_ACTION_MODUS;


bool Buttons::powerup() {

    pinMode(Buttons::buttonA.gpin, INPUT_PULLDOWN);
    attachInterrupt(Buttons::buttonA.ipin, Buttons::handleInterruptA, CHANGE);

    pinMode(Buttons::buttonB.gpin, INPUT_PULLDOWN);
    attachInterrupt(Buttons::buttonB.ipin, Buttons::handleInterruptB, CHANGE);

    pinMode(Buttons::buttonC.gpin, INPUT_PULLUP);
    attachInterrupt(Buttons::buttonC.ipin, Buttons::handleInterruptC, CHANGE);
    return true;

}


void Buttons::handleInterruptA() {
    uint64_t interruptMillis = millis();
    if (digitalRead(Buttons::buttonA.gpin) == LOW && (interruptMillis - Buttons::buttonA.lastInterruptMillis) > BUTTON_DEBOUNCE_MILLIS) {
        if (!Display::exceedsDispActiveDuration(millis())) { // ignore actual action if display is inactive
            if (Buttons::buttonAction == BUTTON_ACTION_MODUS) {
                modus_________e currModus = Device::getCurrModus();
                if (currModus == MODUS________CHARS) {
                    Device::setCurrModus(MODUS________WORDS);
                } else if (currModus == MODUS________WORDS) {
                    Device::setCurrModus(MODUS________LABEL);
                } else if (currModus == MODUS________LABEL) {
                    Device::setCurrModus(MODUS________FREQU);
                } else if (currModus == MODUS________FREQU) {
                    Device::setCurrModus(MODUS________BREAK);
                } else if (currModus == MODUS________BREAK) {
                    Device::setCurrModus(MODUS________PARTY);
                } else if (currModus == MODUS________PARTY) {
                    Device::setCurrModus(MODUS________ACCEL);
                } else {
                    Device::setCurrModus(MODUS________CHARS);
                }
                Blesrv::writeModus();  // send the new value over BLE (when connected)
            } else if (Buttons::buttonAction == BUTTON_ACTION_DECAY && Microphone::decay < 80) {
                Microphone::decay += 5;
            } else if (Buttons::buttonAction == BUTTON_ACTION_LIGHT && Matrices::setBrightness(Matrices::getBrightness() + 1)) {
                Blesrv::writeLight();
            } else if (Orientation::setCoefficientThreshold(min(COEFFICIENT_THRES__MAX, Orientation::getCoefficientThreshold() + 0.05))) {
                Blesrv::writeCoeff();
            }
        }
        Buttons::buttonA.lastInterruptMillis = interruptMillis;
        Display::setNeedsConfigRedraw();
    }
}

void Buttons::handleInterruptB() {
    uint64_t interruptMillis = millis();
    if (digitalRead(Buttons::buttonB.gpin) == HIGH && (interruptMillis - Buttons::buttonB.lastInterruptMillis) > BUTTON_DEBOUNCE_MILLIS) {
        if (!Display::exceedsDispActiveDuration(millis())) { // ignore actual action if display is inactive
            if (Buttons::buttonAction == BUTTON_ACTION_MODUS) {
                Buttons::buttonAction = BUTTON_ACTION_DECAY;
            } else if (Buttons::buttonAction == BUTTON_ACTION_DECAY) {
                Buttons::buttonAction = BUTTON_ACTION_LIGHT;
            } else if (Buttons::buttonAction == BUTTON_ACTION_LIGHT) {
                Buttons::buttonAction = BUTTON_ACTION_COEFF;
            } else {
                Buttons::buttonAction = BUTTON_ACTION_MODUS;
            }
        }
        Buttons::buttonB.lastInterruptMillis = interruptMillis;
        Display::setNeedsConfigRedraw();
    }
}

void Buttons::handleInterruptC() {
    uint64_t interruptMillis = millis();
    if (digitalRead(Buttons::buttonC.gpin) == HIGH && (interruptMillis - Buttons::buttonC.lastInterruptMillis) > BUTTON_DEBOUNCE_MILLIS) {
        if (!Display::exceedsDispActiveDuration(millis())) { // ignore actual action if display is inactive
            if (Buttons::buttonAction == BUTTON_ACTION_MODUS) {
                modus_________e currModus = Device::getCurrModus();
                if (currModus == MODUS________ACCEL) {
                    Device::setCurrModus(MODUS________PARTY);
                } else if (currModus == MODUS________PARTY) {
                    Device::setCurrModus(MODUS________BREAK);
                } else if (currModus == MODUS________BREAK) {
                    Device::setCurrModus(MODUS________FREQU);
                } else if (currModus == MODUS________FREQU) {
                    Device::setCurrModus(MODUS________LABEL);
                } else if (currModus == MODUS________LABEL) {
                    Device::setCurrModus(MODUS________WORDS);
                } else if (currModus == MODUS________WORDS) {
                    Device::setCurrModus(MODUS________CHARS);
                } else {
                    Device::setCurrModus(MODUS________ACCEL);
                }
                Blesrv::writeModus();  // send the new value over BLE (when connected)
            } else if (Buttons::buttonAction == BUTTON_ACTION_DECAY && Microphone::decay > 5) {
                Microphone::decay -= 5;
            } else if (Buttons::buttonAction == BUTTON_ACTION_LIGHT && Matrices::setBrightness(Matrices::getBrightness() - 1)) {
                Blesrv::writeLight();
            } else if (Orientation::setCoefficientThreshold(max(COEFFICIENT_THRES__MIN, Orientation::getCoefficientThreshold() - 0.05))) {
                Blesrv::writeCoeff();
            }
        }
        Buttons::buttonC.lastInterruptMillis = interruptMillis;
        Display::setNeedsConfigRedraw();
    }
}
