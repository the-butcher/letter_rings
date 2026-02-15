#include "Buttons.h"

Button Buttons::buttonA(GPIO_NUM_2);
Button Buttons::buttonB(GPIO_NUM_1);
Button Buttons::buttonC(GPIO_NUM_0);
button_action_e Buttons::buttonActionA = BUTTON_ACTION_MODUS;

void Buttons::handleInterruptA() {
    uint64_t interruptMillis = millis();
    if (digitalRead(Buttons::buttonA.gpin) == LOW && (interruptMillis - Buttons::buttonA.lastInterruptMillis) > BUTTON_DEBOUNCE_MILLIS) {
        if (Buttons::buttonActionA == BUTTON_ACTION_MODUS) {
            if (Device::modus == MODUS________WORDS) {
                Device::modus = MODUS________LABEL;
            } else if (Device::modus == MODUS________LABEL) {
                Device::modus = MODUS________FREQU;
            } else {
                Device::modus = MODUS________WORDS;
            }
            Blesrv::writeModus();  // send the new value over BLE (when connected)
        } else if (Buttons::buttonActionA == BUTTON_ACTION_DECAY && Microphone::decay < 80) {
            Microphone::decay += 5;
        } else if (Matrices::brightness < 15) {
            Matrices::brightness += 1;
            Matrices::needsBrightnessUpdate = true;
        }
        Buttons::buttonA.lastInterruptMillis = interruptMillis;
        Display::needsStatusRedraw = true;
    }
}

void Buttons::handleInterruptB() {
    uint64_t interruptMillis = millis();
    if (digitalRead(Buttons::buttonB.gpin) == HIGH && (interruptMillis - Buttons::buttonB.lastInterruptMillis) > BUTTON_DEBOUNCE_MILLIS) {
        if (Buttons::buttonActionA == BUTTON_ACTION_MODUS) {
            Buttons::buttonActionA = BUTTON_ACTION_DECAY;
        } else if (Buttons::buttonActionA == BUTTON_ACTION_DECAY) {
            Buttons::buttonActionA = BUTTON_ACTION_LIGHT;
        } else {
            Buttons::buttonActionA = BUTTON_ACTION_MODUS;
        }
        Buttons::buttonB.lastInterruptMillis = interruptMillis;
        Display::needsStatusRedraw = true;
    }
}

void Buttons::handleInterruptC() {
    uint64_t interruptMillis = millis();
    if (digitalRead(Buttons::buttonC.gpin) == HIGH && (interruptMillis - Buttons::buttonC.lastInterruptMillis) > BUTTON_DEBOUNCE_MILLIS) {
        if (Buttons::buttonActionA == BUTTON_ACTION_MODUS) {
            if (Device::modus == MODUS________FREQU) {
                Device::modus = MODUS________LABEL;
            } else if (Device::modus == MODUS________LABEL) {
                Device::modus = MODUS________WORDS;
            } else {
                Device::modus = MODUS________FREQU;
            }
            Blesrv::writeModus();  // send the new value over BLE (when connected)
        } else if (Buttons::buttonActionA == BUTTON_ACTION_DECAY && Microphone::decay > 5) {
            Microphone::decay -= 5;
        } else if (Matrices::brightness > 0) {
            Matrices::brightness -= 1;
            Matrices::needsBrightnessUpdate = true;
        }
        Buttons::buttonC.lastInterruptMillis = interruptMillis;
        Display::needsStatusRedraw = true;
    }
}

bool Buttons::powerup() {
    pinMode(Buttons::buttonA.gpin, INPUT_PULLDOWN);
    attachInterrupt(Buttons::buttonA.ipin, Buttons::handleInterruptA, CHANGE);

    pinMode(Buttons::buttonB.gpin, INPUT_PULLDOWN);
    attachInterrupt(Buttons::buttonB.ipin, Buttons::handleInterruptB, CHANGE);

    pinMode(Buttons::buttonC.gpin, INPUT_PULLUP);
    attachInterrupt(Buttons::buttonC.ipin, Buttons::handleInterruptC, CHANGE);
    return true;
}
