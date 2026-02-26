#ifndef Buttons_h
#define Buttons_h

#include <Arduino.h>
#include <Define.h>

#include "Button.h"
#include "Display.h"
#include "Microphone.h"

class Buttons {
   private:
   public:
    static button_action_e buttonActionA;  // TODO :: should not have the "A" postfix
    static Button buttonA;
    static Button buttonB;
    static Button buttonC;
    static void handleInterruptA();
    static void handleInterruptB();
    static void handleInterruptC();
    static bool powerup();
};

#endif