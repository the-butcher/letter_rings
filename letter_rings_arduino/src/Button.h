#ifndef Button_h
#define Button_h

#include <Arduino.h>

class Button {
   private:
   public:
    Button(gpio_num_t gpin);
    gpio_num_t gpin;
    uint8_t ipin;
    uint64_t lastInterruptMillis;
};

#endif