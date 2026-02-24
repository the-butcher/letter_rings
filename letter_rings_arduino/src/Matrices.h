#ifndef Matrices_h
#define Matrices_h

#include <Arduino.h>

#include "Define.h"
#include "Device.h"
#include "Matrix.h"

class Matrices {
   private:
   public:
    static bool needsBrightnessUpdate;
    static void updateBrightness();

    static uint8_t brightness;
    static Matrix matrixA;
    static Matrix matrixB;
    static Matrix matrixC;
    static Matrix matrixD;

    static bool powerup();
    static bool depower();
    static void drawBars();
    static void clear();
    static void write();
    static void setOrientation(orientation___e orientation);
    static void drawWord(String word);
    static void drawLabel(String label, int16_t offset);
    static void drawBitmap(const uint8_t* bitmap, int16_t offset, uint16_t color, orientation___e orientation);
    // static void drawTextToCanvasA(String text);
    // static void copyCanvasAtoCanvasB(uint8_t progress, bool skipPrevious);
    // static void flushCanvasBtoMatrix();
};

#endif