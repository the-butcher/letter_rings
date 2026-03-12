#ifndef GamOL_h
#define GamOL_h

#include <Arduino.h>
#include "Define.h"

class GamOL {
private:
    /**
     * the game field current state
     * 1 bit  per pixel (0-1)
     * 08 uint8_t values can hold one  8x8 matrix   (8 rows / 8 bits)
     * 32 uint8_t values can hold four 8x8 matrices (8 rows / 8 bits)
     */
    static uint8_t fieldState[32]; // 8 uint8_t
    /**
     * the game field current neighbour counts
     * 4 bits per pixel (0-15)
     */
    static uint32_t fieldCount[32]; // 8 uint32_t

public:
    static bool begin();
    /**
     * read the current field state from what is displayed on the matrices
     * should only be done once when entering GAMOL mode
     */
    static void readFieldState();
    /**
     * - get new neighbour counts based on field state
     * - get new field state based on neighbour counts
     */
    static void stepFieldState();
    /**
     * write the current field state to the matrices
     */
    static void drawFieldState();

};

#endif