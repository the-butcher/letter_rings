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
    Adafruit_8x8matrix baseMatrix;
    /**
     * internal canvas that stores pixels that need to be turned on
     */
    GFXcanvas1 writeCanvas;
    /**
     * internal canvas that stores pixels that need to be turned off
     */
    GFXcanvas1 clearCanvas;
    uint8_t addr;
    String name;

public:

    Matrix(uint8_t addr, String name);
    void setBrightness(uint8_t brightness);  // 0 - 15
    void setOrientation(orientation___e orientation);
    uint16_t getWordWidth(String word);
    uint16_t getLabelWidth(String label);
    void drawChar(char__________t character, int16_t offset);
    void drawWord(String word, int16_t offset);
    void drawLabel(String label, int16_t offset);
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void drawBars(uint8_t indexMin);
    void drawBitmap(const uint8_t* bitmap, int16_t offset, uint16_t color, uint8_t width);

    /**
     * clear canvases and or display
     * CLEAR_MATRIX_CANVAS, clear canvases only
     * CLEAR_MATRIX___DISP, clear display only
     * CLEAR_MATRIX_CANVAS | CLEAR_MATRIX___DISP, clear canvases and display
     */
    void clear(uint8_t flags);
    void write();

    uint8_t* getBuffer();

    bool powered;
    bool powerup();
    bool depower();


};

#endif