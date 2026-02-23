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
    GFXcanvas1 canvasA;
    GFXcanvas1 canvasB;
    uint8_t addr;

   public:
    bool hasBegun;
    // uint8_t copyOrder[64];
    Matrix(uint8_t addr);
    bool powerup();
    void setOrientation(orientation___e orientation);
    uint16_t getWordWidth(String word);
    uint16_t getLabelWidth(String label);
    void drawWord(String word, int16_t offset);
    void drawLabel(String label, int16_t offset);
    void clear();
    void write();
    // void drawTextToCanvasA(String text, int16_t offset);
    // void copyCanvasAtoCanvasB(uint8_t progress, bool skipPrevious);
    // void flushCanvasBtoMatrix();
    void drawBars(uint8_t indexMin);
    void drawBitmap(const uint8_t* bitmap, int16_t offset, uint16_t color);
    void setBrightness(uint8_t brightness);  // 0 - 15
    bool depower();
};

#endif