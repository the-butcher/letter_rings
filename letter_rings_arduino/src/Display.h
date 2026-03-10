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

    /**
     * the Adafruit base display
     */
    static Adafruit_ST7789 baseDisplay;

    /**
     * a canvas in display dimensions used to draw arbitrary things to
     */
    static GFXcanvas16 drawCanvas;

    /**
     * a canvas smaller than the display containing only the extent actually needed to be drawn
     * a two stage canvas strategy was chosen to minimize duration and voltage-chatter
     */
    static GFXcanvas16 clipCanvas;

    static bool needsClip;
    static bool needsCopy;
    static extent________t clipExtent;
    static extent________t copyExtent;

    static void drawString(String text, uint16_t x, uint16_t y, text_halign___e halign, uint8_t hPadding = 0, uint16_t color = ST77XX_BLACK);
    static void addClip(uint16_t clipXMin, uint16_t clipYMin, uint16_t clipXMax, uint16_t clipYMax);

    static bool needsConfigRedraw;

    /**
     * last config redraw time
     * this time could be used to measure display activity timeout
     */
    static uint64_t lastConfigRedrawMillis;
    static bool backlightActive;

    /**
     * flags for optimizing things that need only to be drawn a single time
     * labels for orientation and signal
     * matrix states are evaluated only once too
     */
    static bool isFirstDrawOrientation;
    static bool isFirstDrawSignal;
    static bool isFirstDrawMatrixState;

    static bool lastConnectionState;
    static device_role___e lastDeviceRole;
    static String lastText;
    static uint64_t lastSignal;

    /**
     * height of either frequency or acceleration bars
     */
    static uint16_t lastBarsHeight;

public:
    static bool powerup();
    static bool depower();

    static bool exceedsDispActiveDuration(uint64_t currMillis);

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

    /**
     * copy the clip extent to a smaller copy extent
     * the copy extent wis actually written to the display
     */
    static bool writeClip();
    /**
     * write actual canvas updates to the display, if required
     */
    static bool writeCopy();
};

#endif