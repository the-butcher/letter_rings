#ifndef Display_h
#define Display_h

#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ST7789.h>  // Hardware-specific library for ST7789
#include <Arduino.h>
#include <SPI.h>

#include "Blesrv.h"
#include "Buttons.h"
#include "Define.h"
#include "Device.h"
#include "Microphone.h"
#include "Orientation.h"

class Display {
   private:
    static Adafruit_ST7789 baseDisplay;

   public:
    static bool needsStatusRedraw;
    static bool powerup();
    static void drawStatus(modus_________e modus);
    static void drawConfig();

    static void clearModus();
    static void drawText(String label);
    static void drawFrequ();

    static void drawOrientation();
    static void drawConnection();
    static void drawMatrixState();
    static void drawDeviceRole();
    static void drawAcceleration();

    static void drawSignal();
    static bool depower();
    static void drawString(String text, uint16_t x, uint16_t y, text_halign___e halign, uint8_t hPadding = 0, uint16_t color = ST77XX_BLACK);
};

#endif