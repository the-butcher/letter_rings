#ifndef GamOL_h
#define GamOL_h

#include <Arduino.h>

#include "Define.h"
#include "Matrices.h"

class GamOL {
private:
    /**
     * the game field current state
     * 1 bit  per pixel, 2 values (0-1)
     * 08 uint8_t values can hold one  8x8 matrix   (8 rows / 8 bits)
     * 32 uint8_t values can hold four 8x8 matrices (8 rows / 8 bits)
     */
    static uint32_t fieldState[8]; // 8 uint8_t * 4
    /**
     * the game field current neighbour counts
     * 4 bits per pixel, 16 values (0-7)
     */
    static uint8_t fieldCount[8][32]; // 8 uint32_t * 4

public:
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
     * helper method to increase the alive neighbour count for a given cell
     */
    static void increaseCount(uint8_t x, uint8_t y);
    /**
     * write the current field state to the matrices
     */
    static void drawFieldState();

};

#endif