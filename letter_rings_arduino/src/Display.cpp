#include <Display.h>

Adafruit_ST7789 Display::baseDisplay(TFT_CS, TFT_DC, TFT_RST);
GFXcanvas16 Display::drawCanvas(DISPLAY__WIDTH, DISPLAY_HEIGHT);
GFXcanvas16 Display::clipCanvas(1, 1);

bool Display::needsClip = false;
bool Display::needsCopy = false;
extent________t Display::clipExtent = { DISPLAY__WIDTH, DISPLAY_HEIGHT, 0, 0 };
extent________t Display::copyExtent = { 0, 0, 0, 0 };

bool Display::needsConfigRedraw = true;
uint64_t Display::lastConfigRedrawMillis = 0;
bool Display::backlightActive = true;

bool Display::isFirstDrawOrientation = true;
bool Display::isFirstDrawSignal = true;
bool Display::isFirstDrawMatrixState = true;

bool Display::lastConnectionState = true; // start with true to force an initial draw
device_role___e Display::lastDeviceRole = DEVICE_ROLE_____PRI; // start with primary to force an initial draw
uint16_t Display::lastBarsHeight = 112;
String Display::lastText = "";
uint64_t Display::lastSignal = 0;

bool Display::powerup() {

    // turn on backlite
    pinMode(TFT_BACKLITE, OUTPUT);
    digitalWrite(TFT_BACKLITE, HIGH);

    // turn on the TFT / I2C power supply
    pinMode(TFT_I2C_POWER, OUTPUT);
    digitalWrite(TFT_I2C_POWER, HIGH);

    Display::baseDisplay.init(DISPLAY__WIDTH, DISPLAY_HEIGHT);  // Init ST7789 240x135
    Display::baseDisplay.setRotation(0);                        // buttons at the bottom
    Display::baseDisplay.fillScreen(ST77XX_BLACK);

    Display::drawCanvas.setTextWrap(true);

    return true;

}

bool Display::depower() {

    // turn off backlite
    pinMode(TFT_BACKLITE, OUTPUT);
    digitalWrite(TFT_BACKLITE, LOW);

    // turn off the TFT / I2C power supply
    pinMode(TFT_I2C_POWER, OUTPUT);
    digitalWrite(TFT_I2C_POWER, LOW);

    return true;

}

bool Display::exceedsDispActiveDuration(uint64_t currMillis) {
    bool isExceedsDispActiveDuration = (currMillis - Display::lastConfigRedrawMillis) > DISP_ACTIVE_DURATION_MS;
    if (isExceedsDispActiveDuration == Display::backlightActive) { // exceeds = true = backlightActive -> needs to be turned off and vice versa
#if USE__FORCE_BACKLITE_ON == true
        digitalWrite(TFT_BACKLITE, HIGH); // keep backlite on, even when inactive
#else
        digitalWrite(TFT_BACKLITE, isExceedsDispActiveDuration ? LOW : HIGH); // turn off backlite when display is inactive
#endif
        Display::backlightActive = !isExceedsDispActiveDuration; // exceeds = true -> backlightActive should be false
    }
    return isExceedsDispActiveDuration;
}

void Display::setNeedsConfigRedraw() {
    Display::needsConfigRedraw = true;
}

void Display::drawString(String text, uint16_t x, uint16_t y, text_halign___e halign, uint8_t hPadding, uint16_t color) {

    Display::drawCanvas.setCursor(0, 0);
    int16_t x1, y1;
    uint16_t w, h, xAlign;
    Display::drawCanvas.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
    if (halign == TEXT_HALIGN___LEFT) {
        xAlign = x;
    } else if (halign == TEXT_HALIGN_CENTER) {
        xAlign = x - w / 2;
    } else {
        xAlign = x - w;
    }
    Display::drawCanvas.fillRect(xAlign - hPadding - 1, y - 1, w + hPadding * 2, h + 1, color);
    Display::drawCanvas.setCursor(xAlign, y);
    Display::drawCanvas.print(text);

    Display::needsClip = true;

}

void Display::drawConfig() {

    if (Display::needsConfigRedraw) {

        Display::lastConfigRedrawMillis = millis();
        Display::exceedsDispActiveDuration(Display::lastConfigRedrawMillis); // effectively wake the display

        Display::drawCanvas.setTextSize(2);
        Display::drawCanvas.setTextColor(0xad55);  // #AAAAAA

        uint8_t configYPos = DISPLAY_HEIGHT - 14;

        Display::drawString("-", 0, configYPos, TEXT_HALIGN___LEFT);
        String baText;
        if (Buttons::buttonAction == BUTTON_ACTION_MODUS) {
            baText = "MODUS";
        } else if (Buttons::buttonAction == BUTTON_ACTION_DECAY) {
            baText = "DECAY";
        } else if (Buttons::buttonAction == BUTTON_ACTION_LIGHT) {
            baText = "LIGHT";
        } else {
            baText = "COEFF";
        }
        Display::drawString(baText, DISPLAY__WIDTH / 2, configYPos, TEXT_HALIGN_CENTER);
        Display::drawString("+", DISPLAY__WIDTH, configYPos, TEXT_HALIGN__RIGHT);

        String value;
        if (Buttons::buttonAction == BUTTON_ACTION_MODUS) {
            modus_________e currModus = Device::getCurrModus();
            if (currModus == MODUS________WORDS) {
                value = "WORDS";
            } else if (currModus == MODUS________LABEL) {
                value = "LABEL";
            } else if (currModus == MODUS________FREQU) {
                value = "FREQU";
            } else if (currModus == MODUS________BREAK) {
                value = "BREAK";
            } else if (currModus == MODUS________PARTY) {
                value = "PARTY";
            } else if (currModus == MODUS________ACCEL) {
                value = "ACCEL";
            } else {
                value = "ERROR";
            }
        } else if (Buttons::buttonAction == BUTTON_ACTION_DECAY) {
            value = String(Microphone::decay);
        } else if (Buttons::buttonAction == BUTTON_ACTION_LIGHT) {
            value = String(Matrices::getBrightness());
        } else {
            value = String((int)round(Orientation::getCoefficientThreshold() * 100));
        }

        Display::drawString(value, DISPLAY__WIDTH / 2, configYPos - 17, TEXT_HALIGN_CENTER, DISPLAY__WIDTH / 2);

        Display::drawString(DEVICE____________SIDE, DISPLAY__WIDTH, configYPos - 17, TEXT_HALIGN__RIGHT);

        Display::needsConfigRedraw = false;

        Display::addClip(0, configYPos - 17, DISPLAY__WIDTH, DISPLAY_HEIGHT);

        Display::needsClip = true;

    }

}

void Display::clearModus() {
    Display::drawCanvas.fillRect(0, 12, DISPLAY__WIDTH, 120, ST77XX_BLACK);
    Display::addClip(0, 12, DISPLAY__WIDTH, 12 + 120);
    Display::needsClip = true;
}

void Display::drawText(String text) {

    if (!Display::exceedsDispActiveDuration(millis()) && text != Display::lastText) {

        Display::clearModus(); // adds a clip

        Display::drawCanvas.setTextSize(2);
        Display::drawCanvas.setTextColor(ST77XX_WHITE);

        Display::drawString(text, 0, 20, TEXT_HALIGN___LEFT, DISPLAY__WIDTH);

        Display::lastText = text;

        Display::needsClip = true;

    }

}

void Display::drawFrequ() {

    if (!Display::exceedsDispActiveDuration(millis())) {

        uint16_t x;
        uint16_t h;
        const uint16_t y = 124;
        const uint16_t m = 112;

        double scale = 256.0;

        uint16_t clearDisplayHeight = Display::lastBarsHeight;
        Display::drawCanvas.fillRect(0, y - clearDisplayHeight, DISPLAY__WIDTH, clearDisplayHeight + 3, ST77XX_BLACK); // clear previous bars area

        Display::lastBarsHeight = 0;

        // actual band values
        for (int i = 0; i < AUDIO________NUM_BANDS; i++) {
            x = i * 8 + 3;
            h = min(m, (uint16_t)round(Microphone::bandValues[i] / scale));
            Display::drawCanvas.fillRect(x, y - h, 7, h, 0x9cd3);  // #9c9a9c - draw fresh bar
            Display::lastBarsHeight = max(Display::lastBarsHeight, h);
        }


        Display::drawCanvas.drawFastHLine(0, y - (int)round(Microphone::basis / scale), DISPLAY__WIDTH, ST77XX_CYAN);
        // Display::canvas.drawFastHLine(0, y - (int)round(Microphone::fitFAverag / scale), DISPLAY__WIDTH, ST77XX_MAGENTA);

        // the cubic fit curve
        double x0;
        double y0;
        double x1;
        double y1;
        uint16_t h0;
        uint16_t h1;
        for (uint16_t i = 0; i < AUDIO________NUM_BANDS; i++) {
            x1 = i;
            y1 = Microphone::fitFValues[i];
            if (i > 0) {
                h0 = round(min(m * 1.0, y0 / scale));
                h1 = round(min(m * 1.0, y1 / scale));
                Display::drawCanvas.drawLine(round(x0 * 8 + 6), y - h0, round(x1 * 8 + 6), y - h1, ST77XX_YELLOW);
                Display::lastBarsHeight = max(Display::lastBarsHeight, h0);
                Display::lastBarsHeight = max(Display::lastBarsHeight, h1);
            }
            x0 = x1;
            y0 = y1;
        }

        // for (uint16_t i = 0; i < AUDIO________NUM_BANDS; i++) {
        //     x1 = i;
        //     y1 = Microphone::curvValues[i] * 1500;
        //     if (i > 0) {
        //         h0 = round(min(m * 1.0, y0 / scale));
        //         h1 = round(min(m * 1.0, y1 / scale));
        //         Display::drawCanvas.drawLine(round(x0 * 8 + 6), y - h0, round(x1 * 8 + 6), y - h1, ST77XX_MAGENTA);
        //         Display::lastBarsHeight = max(Display::lastBarsHeight, h0);
        //         Display::lastBarsHeight = max(Display::lastBarsHeight, h1);
        //     }
        //     x0 = x1;
        //     y0 = y1;
        // }

        // the scaled/displayed band bar
        for (uint16_t i = 0; i < AUDIO________NUM_BANDS; i++) {
            x = i * 8 + 3;
            h = min(m, (uint16_t)round(Microphone::bandScaled[i] / scale));
            Display::drawCanvas.drawRect(x, y - h, 7, h, 0xdefb);  // #dddddd - draw scaled bar
            Display::lastBarsHeight = max(Display::lastBarsHeight, h);
        }

        Display::addClip(0, y - max(clearDisplayHeight, Display::lastBarsHeight), DISPLAY__WIDTH, y);

        Display::needsClip = true;

    }

}

void Display::drawAcceleration() {

    if (!Display::exceedsDispActiveDuration(millis())) {

        uint16_t x;
        uint16_t h;
        const uint16_t y = 124;
        const uint16_t m = 112;

        uint16_t clearDisplayHeight = Display::lastBarsHeight;
        Display::drawCanvas.fillRect(0, y - clearDisplayHeight, DISPLAY__WIDTH, clearDisplayHeight, ST77XX_BLACK); // clear previous bars area

        Display::drawCanvas.setTextSize(2);
        Display::drawCanvas.setTextColor(Orientation::isAboveCoefficientThreshold() ? 0xad55 : 0xf800);  // #adaaad : #ff0000
        Display::drawString(String(Orientation::getCoefficient(), 3), 0, 20, TEXT_HALIGN___LEFT, DISPLAY__WIDTH);

        acceleration__t accelA = Orientation::getAccelA();
        for (uint16_t i = 0; i < ACCELERATION___SAMPLES; i++) {
            x = i * 4 + 3;
            h = min(m, (uint16_t)round(accelA.values[i] * 32));
            Display::drawCanvas.fillRect(x, y - h, 3, h, 0x9cd3);  // #9c9a9c - draw fresh bar (grey)
        }

        acceleration__t accelB = Orientation::getAccelB();
        for (uint16_t i = 0; i < ACCELERATION___SAMPLES; i++) {
            x = i * 4 + 3;
            h = min(m, (uint16_t)round(accelB.values[i] * 32));
            Display::drawCanvas.drawRect(x, y - h, 3, h, 0xdefb);  // #dddddd - draw scaled bar
        }

        Display::lastBarsHeight = 112;
        Display::addClip(0, y - max(clearDisplayHeight, Display::lastBarsHeight), DISPLAY__WIDTH, y);

        Display::needsClip = true;

    }

}

void Display::drawMatrixState() {

    if (Display::isFirstDrawMatrixState) { // no need to check for exceedsDispActiveDuration, since called only once

        uint8_t rectXPos = 0;
        uint8_t rectYPos = DISPLAY_HEIGHT - 27;
        Display::drawCanvas.fillRect(rectXPos, rectYPos, 6, 6, Matrices::matrixA.powered ? ST77XX_GREEN : ST77XX_RED);
        rectXPos += 8;
        Display::drawCanvas.fillRect(rectXPos, rectYPos, 6, 6, Matrices::matrixB.powered ? ST77XX_GREEN : ST77XX_RED);
        rectXPos += 8;
        Display::drawCanvas.fillRect(rectXPos, rectYPos, 6, 6, Matrices::matrixC.powered ? ST77XX_GREEN : ST77XX_RED);
        rectXPos += 8;
        Display::drawCanvas.fillRect(rectXPos, rectYPos, 6, 6, Matrices::matrixD.powered ? ST77XX_GREEN : ST77XX_RED);

        Display::addClip(0, rectYPos, 32, rectYPos + 6);

        Display::isFirstDrawMatrixState = false;

        Display::needsClip = true;

    }

}

void Display::drawDeviceRole() {

    device_role___e deviceRole = Device::getDeviceRole();

    if (!Display::exceedsDispActiveDuration(millis()) && deviceRole != Display::lastDeviceRole) {

        uint16_t roleColors[3] = { 0xce59, ST77XX_YELLOW, ST77XX_MAGENTA };

        uint8_t rectXPos = DISPLAY__WIDTH - 29;
        uint8_t rectYPos = 0;
        for (uint8_t i = DEVICE_ROLE_____ANY; i <= DEVICE_ROLE_____SEC; i++) {
            Display::drawCanvas.fillRect(rectXPos, rectYPos, 9, 9, ST77XX_BLACK);
            if (deviceRole == i) {
                Display::drawCanvas.fillRect(rectXPos, rectYPos, 9, 9, roleColors[i]);
            } else {
                Display::drawCanvas.drawRect(rectXPos, rectYPos, 9, 9, 0x9cd3);
            }
            rectXPos += 10;
        }

        Display::addClip(DISPLAY__WIDTH - 29, 0, DISPLAY__WIDTH, 10);

        Display::lastDeviceRole = deviceRole;

        Display::needsClip = true;

    }

}

void Display::drawConnection() {

    bool connected = Blesrv::isConnected();

    if (!Display::exceedsDispActiveDuration(millis()) && connected != Display::lastConnectionState) {

        Display::drawCanvas.setTextSize(1);
        Display::drawCanvas.setTextColor(connected ? ST77XX_WHITE : 0x73ae);  // https://rgbcolorpicker.com/565

        uint16_t yPosC = 1;
        Display::drawString(Blesrv::macAdress, 2, yPosC, TEXT_HALIGN___LEFT, 2, connected ? ST77XX_BLUE : 0x31a6);

        Display::addClip(0, 0, DISPLAY__WIDTH, 10);

        Display::lastConnectionState = connected;

        Display::needsClip = true;

    }

}

void Display::drawOrientation() {

    if (!Display::exceedsDispActiveDuration(millis()) && Orientation::powered) {

        Display::drawCanvas.setTextSize(2);
        Display::drawCanvas.setTextColor(Orientation::powered ? 0xad55 : 0xdc30);  // #adaaad : #de8684 // https://rgbcolorpicker.com/565

        vector________t orientation = Orientation::getOrientation();

        // uint8_t xPosValue = 24;
        uint8_t yPosOffset = DISPLAY_HEIGHT - 20;

        // dont draw x since it has no relevance for device functionality
        // uint16_t yPosX = yPosOffset - 17 * 5;
        // Display::drawString(String(orientation.x, 2), xPosValue + getXLabelOffset(orientation.x), yPosX, TEXT_HALIGN___LEFT, 36);
        // Display::drawString("X: ", 0, yPosX, TEXT_HALIGN___LEFT);

        uint16_t yPosY = yPosOffset - 17 * 5;
        Display::drawString(String(orientation.y, 2), DISPLAY__WIDTH, yPosY, TEXT_HALIGN__RIGHT, 36);
        if (Display::isFirstDrawOrientation) {
            Display::drawString("Y: ", 0, yPosY, TEXT_HALIGN___LEFT);
        }

        uint16_t yPosZ = yPosOffset - 17 * 4;
        Display::drawString(String(orientation.z, 2), DISPLAY__WIDTH, yPosZ, TEXT_HALIGN__RIGHT, 36);
        if (Display::isFirstDrawOrientation) {
            Display::drawString("Z: ", 0, yPosZ, TEXT_HALIGN___LEFT);
        }

        uint16_t yPosO = yPosOffset - 17 * 3;
        if (Display::isFirstDrawOrientation) {
            Display::drawString("O: ", 0, yPosO, TEXT_HALIGN___LEFT);
        }
        Display::drawString(Device::getOrientation() == ORIENTATION______UP ? "  UP" : "DOWN", DISPLAY__WIDTH, yPosO, TEXT_HALIGN__RIGHT);

        Display::addClip(Display::isFirstDrawOrientation ? 0 : 45, yPosY, DISPLAY__WIDTH, yPosY + 17 * 3 - 2);

        Display::isFirstDrawOrientation = false;

        Display::needsClip = true;

    }

}

void Display::drawSignal() {

    uint64_t currSignal = Microphone::signal;

    if (!Display::exceedsDispActiveDuration(millis()) && currSignal != Display::lastSignal) {

        Display::drawCanvas.setTextSize(2);
        Display::drawCanvas.setTextColor(0xad55);  // #AAAAAA

        uint8_t yPosOffset = DISPLAY_HEIGHT - 20;

        uint16_t yPosS = yPosOffset - 17 * 2;  // same as o label
        String signalString;
        if (Microphone::signal >= 1000) {
            signalString = String(Microphone::signal);
        } else if (Microphone::signal >= 100) {
            signalString = " " + String(Microphone::signal);
        } else if (Microphone::signal >= 10) {
            signalString = "  " + String(Microphone::signal);
        } else {
            signalString = "   " + String(Microphone::signal);
        }
        if (Display::isFirstDrawSignal) {
            Display::drawString("S: ", 0, yPosS, TEXT_HALIGN___LEFT);
        }
        Display::drawString(signalString, DISPLAY__WIDTH, yPosS, TEXT_HALIGN__RIGHT);

        Display::addClip(Display::isFirstDrawSignal ? 0 : 45, yPosS, DISPLAY__WIDTH, yPosS + 16);

        Display::isFirstDrawSignal = false;
        Display::lastSignal = currSignal;

        Display::needsClip = true;

    }

}

void Display::addClip(uint16_t clipXMin, uint16_t clipYMin, uint16_t clipXMax, uint16_t clipYMax) {
    Display::clipExtent.xMin = min(Display::clipExtent.xMin, clipXMin);
    Display::clipExtent.yMin = min(Display::clipExtent.yMin, clipYMin);
    Display::clipExtent.xMax = max(Display::clipExtent.xMax, clipXMax);
    Display::clipExtent.yMax = max(Display::clipExtent.yMax, clipYMax);
}

bool Display::writeClip() {

    if (Display::needsClip) {

        Display::clipExtent.xMin = max(Display::clipExtent.xMin, (uint16_t)0);
        Display::clipExtent.yMin = max(Display::clipExtent.yMin, (uint16_t)0);
        Display::clipExtent.xMax = min(Display::clipExtent.xMax, DISPLAY__WIDTH);
        Display::clipExtent.yMax = min(Display::clipExtent.yMax, DISPLAY_HEIGHT);

        int16_t clipW = Display::clipExtent.xMax - Display::clipExtent.xMin;
        int16_t clipH = Display::clipExtent.yMax - Display::clipExtent.yMin;

        if (clipW > 0 && clipH > 0) {

            Display::clipCanvas = GFXcanvas16(clipW, clipH);
            clipCanvas.drawRGBBitmap(-(int16_t)(Display::clipExtent.xMin), -(int16_t)(Display::clipExtent.yMin), Display::drawCanvas.getBuffer(), DISPLAY__WIDTH, DISPLAY_HEIGHT);

            Display::copyExtent = { Display::clipExtent.xMin, Display::clipExtent.yMin, Display::clipExtent.xMax, Display::clipExtent.yMax };
            Display::needsCopy = true;

        }

        Display::clipExtent = { DISPLAY__WIDTH, DISPLAY_HEIGHT, 0, 0 };
        Display::needsClip = false;

        return true;

    } else {
        return false;
    }


}

bool Display::writeCopy() {

    if (Display::needsCopy) {

        int16_t copyW = Display::copyExtent.xMax - Display::copyExtent.xMin;
        int16_t copyH = Display::copyExtent.yMax - Display::copyExtent.yMin;

        if (copyW > 0 && copyH > 0) {
            Display::baseDisplay.drawRGBBitmap(Display::copyExtent.xMin, Display::copyExtent.yMin, Display::clipCanvas.getBuffer(), copyW, copyH);
#if USE__________CLIP_DRAW == true
            Display::baseDisplay.drawRect(Display::copyExtent.xMin, Display::copyExtent.yMin, copyW, copyH, ST77XX_RED);
#endif
        }

        Display::needsCopy = false;

        return true;

    } else {
        return false;
    }

}