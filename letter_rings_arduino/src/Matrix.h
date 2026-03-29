#ifndef Matrix_h
#define Matrix_h

#include <Adafruit_GFX.h>  // Core graphics library
#include <Adafruit_LEDBackpack.h>
#include <Arduino.h>

#include "Define.h"
#include "Font7x8FixedMono.h"
#include "Font5x6FixedMono.h"
#include "Microphone.h"

class Matrix {
private:
    /**
     * a reference to the underlying matrix element
     */
    Adafruit_8x8matrix baseMatrix;
    /**
     * the brightness currently set
     */
    uint8_t brightnessCurr;
    /**
     * the brightness that should be set
     */
    uint8_t brightnessPend;
    /**
     * internal canvas that stores pixels that need to be turned on
     */
    GFXcanvas1 writeCanvas;
    /**
     * internal canvas that stores pixels that need to be turned off
     */
    GFXcanvas1 clearCanvas;
    /**
     * the I2C address of the matrix element, depending on how pads are soldered, 0x70 | 0x71 | 0x72 | 0x73
     */
    uint8_t addr;
    /**
     * a name for serial output, A | B | C | D
     */
    String name;


public:

    Matrix(uint8_t addr, String name);
    bool setBrightness(uint8_t brightness);  // 0 - 15
    void setOrientation(orientation___e orientation);

    uint16_t getWordWidth(String word);
    uint16_t getLabelWidth(String label);

    void drawChar(char__________t character, int16_t offset);
    void drawWord(String word, int16_t offset);
    void drawLabel(String label, int16_t offset);

    /**
     * draw a single pixel at the given coordinate
     */
    void drawPixel(int16_t x, int16_t y, uint16_t color);

    /**
     * draw frequency bars starting from the given index
     * actual bar heights are taken from microphone values
     */
    void drawBars(uint8_t indexMin);

    void drawLedbar(uint8_t value);

    /**
     * draw the given bitmap (having the given height) at offset with color (LED_ON | LED_OFF)
     */
    void drawBitmap(const uint8_t* bitmap, int16_t offset, uint16_t color, uint8_t width);

    /**
     * clear canvases and or display
     * CLEAR_MATRIX_CANVAS, clear canvases only
     * CLEAR_MATRIX___DISP, clear display only
     * CLEAR_MATRIX_CANVAS | CLEAR_MATRIX___DISP, clear canvases and display
     */
    void clear(uint8_t flags);

    /**
     * write the current state of clearCanvas and writeCanvas to the baseMatrix
     */
    void write();

    /**
     * get the current buffer (image) of this matrix element
     */
    uint8_t* getBuffer();

    bool powered;
    bool powerup();
    bool depower();


};

#endif