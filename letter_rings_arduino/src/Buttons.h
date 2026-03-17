#ifndef Buttons_h
#define Buttons_h

#include <Arduino.h>

#include "Define.h"
#include "Button.h"
#include "Display.h"
#include "Microphone.h"

class Buttons {
private:
    static Button buttonA;
    static Button buttonB;
    static Button buttonC;
public:
    static button_action_e buttonAction;
    static void handleInterruptA();
    static void handleInterruptB();
    static void handleInterruptC();
    static bool powerup();
};

#endif