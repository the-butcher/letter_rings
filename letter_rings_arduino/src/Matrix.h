#ifndef Matrix_h
#define Matrix_h

#include <Adafruit_GFX.h>  // Core graphics library
#include <Adafruit_LEDBackpack.h>
#include <Arduino.h>

#include "Define.h"
#include "Font7x8FixedMono.h"
#include "Microphone.h"

class Matrix {
private:
    Adafruit_8x8matrix baseMatrix;
    GFXcanvas1 writeCanvas;
    GFXcanvas1 clearCanvas;
    uint8_t addr;
    String name;

public:

    void setBrightness(uint8_t brightness);  // 0 - 15

    bool hasBegun;
    Matrix(uint8_t addr, String name);
    bool powerup();
    bool depower();
    void setOrientation(orientation___e orientation);
    uint16_t getWordWidth(String word);
    uint16_t getLabelWidth(String label);
    void drawWord(String word, int16_t offset);
    void drawLabel(String label, int16_t offset);
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void drawBars(uint8_t indexMin);
    void drawBitmap(const uint8_t* bitmap, int16_t offset, uint16_t color, uint8_t width);

    void clear();
    void clearCanvases();
    void write();

    uint8_t* getBuffer();

};

#endif