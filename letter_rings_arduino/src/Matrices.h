#ifndef Matrices_h
#define Matrices_h

#include <Arduino.h>

#include "Define.h"
#include "Device.h"
#include "Matrix.h"

class Matrices {
private:
    static bool needsWrite;
    /**
     * the brightness currently set
     */
    static uint8_t brightnessCurr;
    /**
     * the brightness that should be set
     */
    static uint8_t brightnessPend;

public:

    /**
     * get the current brightness
     * note: return the pending brightness value
     */
    static uint8_t getBrightness();

    /**
     * sets a new brightness value to the Matrices
     * @return true if the new value is an actual change with respect to the old brightness
     */
    static bool setBrightness(uint8_t brightness);

    static Matrix matrixA;
    static Matrix matrixB;
    static Matrix matrixC;
    static Matrix matrixD;

    static bool powerup();
    static bool depower();

    static void clear();

    static void setOrientation(orientation___e orientation);
    static void drawPixel(int16_t x, int16_t y, uint16_t color);
    static void drawBars();
    static void drawWord(String word);
    static void drawLabel(String label, int16_t offset);
    static void drawBitmap(const uint8_t* bitmap, int16_t offset, uint16_t color, orientation___e orientation);

    /**
     * write any pending changes to content or brightness
     * @return true if changes were made or if the brightness was updated
     */
    static bool write();

};

#endif