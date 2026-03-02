#include <Display.h>

Adafruit_ST7789 Display::baseDisplay(TFT_CS, TFT_DC, TFT_RST);
bool Display::needsStatusRedraw = true;
bool Display::needsWrite = false;
GFXcanvas16 Display::canvas(DISPLAY__WIDTH, DISPLAY_HEIGHT);

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

    Display::canvas.setTextWrap(true);

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

void Display::setNeedsStatusRedraw() {
    Display::needsStatusRedraw = true;
}

bool Display::drawStatus(modus_________e modus) {

    if (Display::needsStatusRedraw) {

        Display::drawConfig();  // buttons labels
        Display::drawConnection();
        Display::drawOrientation();
        if (modus == MODUS________LABEL) {
            Display::drawText(Device::label);
        }
        Display::drawMatrixState();  // I2C init states of matrices

        Display::needsWrite = true;
        Display::needsStatusRedraw = false;

        return true;

    } else {
        return false;
    }

}

void Display::drawString(String text, uint16_t x, uint16_t y, text_halign___e halign, uint8_t hPadding, uint16_t color) {

    Display::canvas.setCursor(0, 0);
    int16_t x1, y1;
    uint16_t w, h, xAlign;
    Display::canvas.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
    if (halign == TEXT_HALIGN___LEFT) {
        xAlign = x;
    } else if (halign == TEXT_HALIGN_CENTER) {
        xAlign = x - w / 2;
    } else {
        xAlign = x - w;
    }
    Display::canvas.fillRect(xAlign - hPadding - 1, y - 1, w + hPadding * 2, h + 1, color);
    Display::canvas.setCursor(xAlign, y);
    Display::canvas.print(text);

    Display::needsWrite = true;

}

void Display::drawConfig() {

    Display::canvas.setTextSize(2);
    Display::canvas.setTextColor(0xad55);  // #AAAAAA

    uint8_t configYPos = DISPLAY_HEIGHT - 14;

    Display::drawString("-", 0, configYPos, TEXT_HALIGN___LEFT);
    String baText;
    if (Buttons::buttonAction == BUTTON_ACTION_MODUS) {
        baText = "MODUS";
    } else if (Buttons::buttonAction == BUTTON_ACTION_DECAY) {
        baText = "DECAY";
    } else {
        baText = "LIGHT";
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
        } else if (currModus == MODUS________PARTY) {
            value = "PARTY";
        } else if (currModus == MODUS________ACCEL) {
            value = "ACCEL";
        } else {
            value = "ERROR";
        }
    } else if (Buttons::buttonAction == BUTTON_ACTION_DECAY) {
        value = String(Microphone::decay);
    } else {
        value = String(Matrices::getBrightness());
    }

    Display::drawString(value, DISPLAY__WIDTH / 2, configYPos - 17, TEXT_HALIGN_CENTER, DISPLAY__WIDTH / 2);

    Display::drawString(DEVICE____________SIDE, DISPLAY__WIDTH, configYPos - 17, TEXT_HALIGN__RIGHT);

    Display::needsWrite = true;
}

void Display::clearModus() {
    Display::canvas.fillRect(0, 12, DISPLAY__WIDTH, 120, ST77XX_BLACK);
    Display::needsWrite = true;
}

void Display::drawText(String label) {

    Display::clearModus();

    Display::canvas.setTextSize(2);
    Display::canvas.setTextColor(ST77XX_WHITE);

    Display::drawString(label, 0, 20, TEXT_HALIGN___LEFT, DISPLAY__WIDTH);

    Display::needsWrite = true;
}

void Display::drawFrequ() {

    int x;
    int h;
    const int y = 124;
    const int m = 112;

    Display::clearModus();

    // actual band values
    for (int i = 0; i < AUDIO________NUM_BANDS; i++) {
        x = i * 8 + 3;
        h = min(m, (int)round(Microphone::bandValues[i] / 256.0));
        Display::canvas.fillRect(x, y - h, 7, h, 0x9cd3);  // #9c9a9c - draw fresh bar
    }

    double x0;
    double y0;
    double x1;
    double y1;
    for (int i = 0; i < AUDIO________NUM_BANDS; i++) {
        x1 = i;
        y1 = Microphone::fitFValues[i];
        if (i > 0) {
            Display::canvas.drawLine(round(x0 * 8 + 6), round(y - min(m * 1.0, y0 / 256.0)), round(x1 * 8 + 6), round(y - min(m * 1.0, y1 / 256.0)), ST77XX_YELLOW);
            // Display::canvas.drawLine(round(x0 * 8 + 6), round(y - y0 / 256.0), round(x1 * 8 + 6), round(y - y1 / 256.0), ST77XX_YELLOW);
        }
        x0 = x1;
        y0 = y1;
    }

    for (int i = 0; i < AUDIO________NUM_BANDS; i++) {
        x = i * 8 + 3;
        h = min(m, (int)round(Microphone::bandScaled[i] / 256.0));
        Display::canvas.drawRect(x, y - h, 7, h, 0xdefb);  // #dddddd - draw scaled bar
    }

    Display::canvas.drawFastHLine(0, y - (int)round(Microphone::basis / 256.0), DISPLAY__WIDTH, ST77XX_YELLOW);
    // Display::canvas.drawFastHLine(0, y - (int)round(Microphone::fitFAverag / 256.0), DISPLAY__WIDTH, ST77XX_MAGENTA);

    Display::needsWrite = true;
}

void Display::drawMatrixState() {

    uint8_t rectXPos = 0;
    uint8_t rectYPos = DISPLAY_HEIGHT - 27;
    Display::canvas.fillRect(rectXPos, rectYPos, 6, 6, Matrices::matrixA.hasBegun ? ST77XX_GREEN : ST77XX_RED);
    rectXPos += 8;
    Display::canvas.fillRect(rectXPos, rectYPos, 6, 6, Matrices::matrixB.hasBegun ? ST77XX_GREEN : ST77XX_RED);
    rectXPos += 8;
    Display::canvas.fillRect(rectXPos, rectYPos, 6, 6, Matrices::matrixC.hasBegun ? ST77XX_GREEN : ST77XX_RED);
    rectXPos += 8;
    Display::canvas.fillRect(rectXPos, rectYPos, 6, 6, Matrices::matrixD.hasBegun ? ST77XX_GREEN : ST77XX_RED);

    Display::needsWrite = true;

}

void Display::drawDeviceRole() {

    device_role___e deviceRole = Device::getDeviceRole();

    uint16_t roleColors[3] = { 0xce59, ST77XX_YELLOW, ST77XX_MAGENTA };

    uint8_t rectXPos = DISPLAY__WIDTH - 38;
    uint8_t rectYPos = DISPLAY_HEIGHT - 27;
    for (uint8_t i = DEVICE_ROLE_____ANY; i <= DEVICE_ROLE_____SEC; i++) {
        Display::canvas.fillRect(rectXPos, rectYPos, 6, 6, ST77XX_BLACK);
        if (deviceRole == i) {
            Display::canvas.fillRect(rectXPos, rectYPos, 6, 6, roleColors[i]);
        } else {
            Display::canvas.drawRect(rectXPos, rectYPos, 6, 6, 0x9cd3);
        }
        rectXPos += 8;
    }

    Display::needsWrite = true;

}

void Display::drawConnection() {

    bool connected = Blesrv::isConnected();

    Display::canvas.setTextSize(1);
    Display::canvas.setTextColor(connected ? ST77XX_WHITE : 0x73ae);  // https://rgbcolorpicker.com/565

    uint16_t yPosC = 1;
    Display::drawString(Blesrv::macAdress, DISPLAY__WIDTH / 2, yPosC, TEXT_HALIGN_CENTER, 20, connected ? ST77XX_BLUE : 0x31a6);

    Display::needsWrite = true;

}

uint8_t getXLabelOffset(double value) {

    uint8_t xLabelOffset = 0;
    if (value >= 0) {  // no minus sign needed
        xLabelOffset += 12;
    }
    if (abs(value) < 100) {
        xLabelOffset += 12;
    }
    if (abs(value) < 10) {
        xLabelOffset += 12;
    }
    return xLabelOffset;

}

void Display::drawOrientation() {

    Display::canvas.setTextSize(2);
    Display::canvas.setTextColor(Orientation::hasBegun ? 0xad55 : 0xdc30);  // #adaaad : #de8684 // https://rgbcolorpicker.com/565

    vector________t orientation = Orientation::getOrientation();

    uint8_t xPosValue = 24;

    uint8_t yPosOffset = DISPLAY_HEIGHT - 20;

    uint16_t yPosX = yPosOffset - 17 * 5;
    Display::drawString(String(orientation.x, 2), xPosValue + getXLabelOffset(orientation.x), yPosX, TEXT_HALIGN___LEFT, 36);
    Display::drawString("X: ", 0, yPosX, TEXT_HALIGN___LEFT);

    uint16_t yPosY = yPosOffset - 17 * 4;
    Display::drawString(String(orientation.y, 2), xPosValue + getXLabelOffset(orientation.y), yPosY, TEXT_HALIGN___LEFT, 36);
    Display::drawString("Y: ", 0, yPosY, TEXT_HALIGN___LEFT);

    uint16_t yPosZ = yPosOffset - 17 * 3;
    Display::drawString(String(orientation.z, 2), xPosValue + getXLabelOffset(orientation.z), yPosZ, TEXT_HALIGN___LEFT, 36);
    Display::drawString("Z: ", 0, yPosZ, TEXT_HALIGN___LEFT);

    uint16_t yPosO = yPosOffset - 17 * 2;
    Display::drawString("O: ", 0, yPosO, TEXT_HALIGN___LEFT);
    Display::drawString(Device::getOrientation() == ORIENTATION______UP ? "UP  " : "DOWN", xPosValue, yPosO, TEXT_HALIGN___LEFT);

    Display::needsWrite = true;

}

void Display::drawAcceleration() {

    Display::clearModus();

    Display::canvas.setTextSize(2);
    Display::canvas.setTextColor(Orientation::coefficient > ACCELERATION_THRESHOLD ? 0xad55 : 0xf800);  // #adaaad : #ff0000
    Display::drawString(String(Orientation::coefficient, 3), 0, 20, TEXT_HALIGN___LEFT, DISPLAY__WIDTH);

    int x;
    int h;
    int y = 124;
    int m = 112;

    acceleration__t accelA = Orientation::getAccelA();
    for (int i = 0; i < ACCELERATION___SAMPLES; i++) {
        x = i * 4 + 3;
        h = min(m, (int)round(accelA.values[i] * 32));
        Display::canvas.fillRect(x, y - h, 3, h, 0x9cd3);  // #9c9a9c - draw fresh bar (grey)
    }

    acceleration__t accelB = Orientation::getAccelB();
    for (int i = 0; i < ACCELERATION___SAMPLES; i++) {
        x = i * 4 + 3;
        h = min(m, (int)round(accelB.values[i] * 32));
        Display::canvas.drawRect(x, y - h, 3, h, 0xdefb);  // #dddddd - draw scaled bar
    }

    Display::needsWrite = true;

}

void Display::drawSignal() {

    Display::canvas.setTextSize(2);
    Display::canvas.setTextColor(0xad55);  // #AAAAAA

    uint8_t barHeight = 70;
    uint8_t yPosOffset = DISPLAY_HEIGHT - 20 - 17;

    uint16_t signal = Microphone::signal * barHeight / 4096;

    uint16_t yPosO = yPosOffset - 17;  // same as o label
    String signalString;
    if (Microphone::signal >= 1000) {
        signalString = " " + String(Microphone::signal);
    } else if (Microphone::signal >= 100) {
        signalString = "  " + String(Microphone::signal);
    } else if (Microphone::signal >= 10) {
        signalString = "   " + String(Microphone::signal);
    } else {
        signalString = "    " + String(Microphone::signal);
    }
    Display::drawString(signalString, DISPLAY__WIDTH - 10, yPosO, TEXT_HALIGN__RIGHT);

    Display::canvas.fillRect(DISPLAY__WIDTH - 3, yPosOffset - barHeight, 3, barHeight, ST77XX_BLACK);  // remove previous line
    Display::canvas.fillRect(DISPLAY__WIDTH - 2, yPosOffset - barHeight, 1, barHeight, 0x632c);        // #666666
    Display::canvas.fillRect(DISPLAY__WIDTH - 3, yPosOffset - signal, 3, signal, 0xad55);              // signal line
    Display::canvas.drawFastHLine(DISPLAY__WIDTH - 8, yPosOffset - barHeight, 8, ST77XX_CYAN);
    Display::canvas.drawFastHLine(DISPLAY__WIDTH - 8, yPosOffset - barHeight / 2, 8, ST77XX_CYAN);
    Display::canvas.drawFastHLine(DISPLAY__WIDTH - 8, yPosOffset, 8, ST77XX_CYAN);

    Display::needsWrite = true;

}

bool Display::write() {

    if (Display::needsWrite) {
        Display::baseDisplay.drawRGBBitmap(0, 0, Display::canvas.getBuffer(), DISPLAY__WIDTH, DISPLAY_HEIGHT);
        Display::needsWrite = false;
        return true;
    } else {
        return false;
    }

}