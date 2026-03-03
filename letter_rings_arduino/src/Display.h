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
    static GFXcanvas16 drawCanvas;
    static GFXcanvas16 clipCanvas;
    static bool needsClip;
    static bool needsCopy;
    static bool writingCopy;

    static void drawString(String text, uint16_t x, uint16_t y, text_halign___e halign, uint8_t hPadding = 0, uint16_t color = ST77XX_BLACK);
    static bool needsConfigRedraw;
    static extent________t clipExtent;
    static extent________t copyExtent;
    static void addClip(uint16_t clipXMin, uint16_t clipYMin, uint16_t clipXMax, uint16_t clipYMax);
    static bool isFirstDrawOrientation;
    static bool isFirstDrawSignal;
    static bool lastConnectionState;
    static device_role___e lastDeviceRole;
    static bool isFirstDrawMatrixState;
    static String lastText;
    static uint64_t lastSignal;
    /**
     * height of either frequency or acceleration bars
     */
    static uint16_t lastBarsHeight;

public:
    static bool powerup();
    static bool depower();

    static void clearModus();

    static void setNeedsConfigRedraw();
    static void drawConfig();

    static void drawText(String text);
    static void drawFrequ();

    static void drawOrientation();
    static void drawConnection();
    static void drawMatrixState();
    static void drawDeviceRole();
    static void drawAcceleration();

    static void drawSignal();

    static bool writeClip();
    /**
     * write actual canvas updates to the display, if required
     */
    static bool writeCopy();
};

#endif