#ifndef Matrices_h
#define Matrices_h

#include <Arduino.h>

#include "Define.h"
#include "Device.h"
#include "Matrix.h"

class Matrices {
private:
    /**
     * internal flag describing the need for an actual write
     * if false (and no brightness update pending), a call to the write method will return false
     */
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
    static bool setBrightness(uint8_t brightness, bool force = false);

    static Matrix matrixA;
    static Matrix matrixB;
    static Matrix matrixC;
    static Matrix matrixD;

    /**
     * clear canvases and or display
     * CLEAR_MATRIX_CANVAS, clear canvases only
     * CLEAR_MATRIX___DISP, clear display only
     * CLEAR_MATRIX_CANVAS | CLEAR_MATRIX___DISP, clear canvases and display
     */
    static void clear(uint8_t flags);

    /**
     * write any pending changes to content or brightness
     * @return true if changes were made or if the brightness was updated
     */
    static bool write();

    static void setOrientation(orientation___e orientation);
    static void drawPixel(int16_t x, int16_t y, uint16_t color);
    static void drawBars();
    static void drawLedbar();
    static void drawChar(char__________t character);
    static void drawWord(String word);
    static void drawLabel(String label, int16_t offset);
    static void drawBitmapWithOrientation(const uint8_t* bitmap, int16_t offset, uint16_t color, orientation___e orientation);
    static void drawBitmap(const uint8_t* bitmap, int16_t offset, uint16_t color);

    static uint32_t* getBuffer();

    static bool powerup();
    static bool depower();

};

#endif