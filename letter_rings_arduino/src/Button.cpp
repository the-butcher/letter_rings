#include "Button.h"

Button::Button(gpio_num_t gpin) {
    this->gpin = gpin;
    this->ipin = digitalPinToInterrupt(gpin);
    this->lastInterruptMillis = 0;
}
