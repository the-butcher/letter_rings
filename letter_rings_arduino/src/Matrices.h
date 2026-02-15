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
    static String label;  // a label to be shown running through all letters of the matrix, depending on hand orienation
    static bool powerup();
    static bool depower();
    static void drawBars();
    // static void drawTextToCanvasA(String text);
    // static void copyCanvasAtoCanvasB(uint8_t progress, bool skipPrevious);
    // static void flushCanvasBtoMatrix();
    static void drawWord(String word);
    static void drawLabel(String label, int16_t offset);
};

#endif