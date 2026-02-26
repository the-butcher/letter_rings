#ifndef Button_h
#define Button_h

#include <Arduino.h>

/**
 * simple wrapper for a device button
 * button handling implemented in Buttons
 */
class Button {
   private:
   public:
    Button(gpio_num_t gpin);
    /**
     * the buttons gpio pin
     */
    gpio_num_t gpin;
    /**
     * the buttons digital interrupt pin
     */
    uint8_t ipin;
    /**
     * last time there was an interrupt
     */
    uint64_t lastInterruptMillis;
};

#endif