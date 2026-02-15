#include <Display.h>

Adafruit_ST7789 Display::baseDisplay(TFT_CS, TFT_DC, TFT_RST);
bool Display::needsStatusRedraw = true;

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

void Display::drawStatus() {

    // Display::baseDisplay.fillScreen(ST77XX_BLACK);
    Display::baseDisplay.setTextWrap(true);

    Display::drawConfig();  // buttons labels
    Display::drawConnection();
    Display::drawOrientation();
    if (Device::modus == MODUS________LABEL) {
        Display::drawText(Matrices::label);
    }
    Display::drawMatrixState();

    Display::needsStatusRedraw = false;
}

void Display::drawString(String text, uint16_t x, uint16_t y, text_halign___e halign, uint8_t hPadding, uint16_t color) {
    Display::baseDisplay.setCursor(0, 0);
    int16_t x1, y1;
    uint16_t w, h, xAlign;
    Display::baseDisplay.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
    if (halign == TEXT_HALIGN___LEFT) {
        xAlign = x;
    } else if (halign == TEXT_HALIGN_CENTER) {
        xAlign = x - w / 2;
    } else {
        xAlign = x - w;
    }
    Display::baseDisplay.fillRect(xAlign - hPadding - 1, y - 1, w + hPadding * 2, h + 1, color);
    Display::baseDisplay.setCursor(xAlign, y);
    Display::baseDisplay.print(text);
}

void Display::drawConfig() {

    Display::baseDisplay.setTextSize(2);
    Display::baseDisplay.setTextColor(0xad55);  // #AAAAAA

    uint8_t configYPos = DISPLAY_HEIGHT - 14;

    Display::drawString("-", 0, configYPos, TEXT_HALIGN___LEFT);
    String baText;
    if (Buttons::buttonActionA == BUTTON_ACTION_MODUS) {
        baText = "MODUS";
    } else if (Buttons::buttonActionA == BUTTON_ACTION_DECAY) {
        baText = "DECAY";
    } else {
        baText = "LIGHT";
    }
    Display::drawString(baText, DISPLAY__WIDTH / 2, configYPos, TEXT_HALIGN_CENTER);
    Display::drawString("+", DISPLAY__WIDTH, configYPos, TEXT_HALIGN__RIGHT);

    String value;
    if (Buttons::buttonActionA == BUTTON_ACTION_MODUS) {
        if (Device::modus == MODUS________WORDS) {
            value = "WORDS";
        } else if (Device::modus == MODUS________LABEL) {
            value = "LABEL";
        } else {
            value = "FREQU";
        }
    } else if (Buttons::buttonActionA == BUTTON_ACTION_DECAY) {
        value = String(Microphone::decay);
    } else {
        value = String(Matrices::brightness);
    }

    Display::drawString(value, DISPLAY__WIDTH / 2, configYPos - 17, TEXT_HALIGN_CENTER, DISPLAY__WIDTH / 2);

    Display::drawString(DEVICE____________SIDE, DISPLAY__WIDTH, configYPos - 17, TEXT_HALIGN__RIGHT);
}

void Display::clearModus() {
    Display::baseDisplay.fillRect(0, 12, DISPLAY__WIDTH, 120, ST77XX_BLACK);
}

void Display::drawText(String label) {

    Display::clearModus();

    Display::baseDisplay.setTextSize(2);
    Display::baseDisplay.setTextColor(ST77XX_WHITE);

    Display::drawString(label, 0, 20, TEXT_HALIGN___LEFT, DISPLAY__WIDTH);
}

void Display::drawFrequ() {

    int x;
    int h;
    int y = 124;
    int m = 112;

    Display::clearModus();
    for (int i = 0; i < AUDIO________NUM_BANDS; i++) {
        x = i * 8 + 3;
        h = min(m, (int)round(Microphone::bandValues[i] / 256.0));
        Display::baseDisplay.fillRect(x, y - h, 7, h, 0x9cd3);  // #9c9a9c - draw fresh bar
    }

    double x0;
    double y0;
    double x1;
    double y1;
    for (int i = 0; i < AUDIO________NUM_BANDS; i++) {
        x1 = i;
        y1 = Microphone::fitFValues[i];
        if (i > 0) {
            Display::baseDisplay.drawLine(round(x0 * 8 + 6), round(y - min(h * 1.0, y0 / 256.0)), round(x1 * 8 + 6), round(y - min(h * 1.0, y1 / 256.0)), ST77XX_YELLOW);
        }
        x0 = x1;
        y0 = y1;
    }

    for (int i = 0; i < AUDIO________NUM_BANDS; i++) {
        x = i * 8 + 3;
        h = min(m, (int)round(Microphone::bandScaled[i] / 256.0));
        Display::baseDisplay.drawRect(x, y - h, 7, h, 0xdefb);  // #dddddd - draw scaled bar
    }

    Display::baseDisplay.drawFastHLine(0, y - (int)round(Microphone::basis / 256.0), DISPLAY__WIDTH, ST77XX_YELLOW);
    // Display::baseDisplay.drawFastHLine(0, y - (int)round(Microphone::fitFAverag / 256.0), DISPLAY__WIDTH, ST77XX_MAGENTA);
}

void Display::drawMatrixState() {

    uint8_t rectXPos = 0;
    uint8_t rectYPos = DISPLAY_HEIGHT - 27;
    Display::baseDisplay.fillRect(rectXPos, rectYPos, 6, 6, Matrices::matrixA.hasBegun ? 0xad55 : ST77XX_RED);
    rectXPos += 8;
    Display::baseDisplay.fillRect(rectXPos, rectYPos, 6, 6, Matrices::matrixB.hasBegun ? 0xad55 : ST77XX_RED);
    rectXPos += 8;
    Display::baseDisplay.fillRect(rectXPos, rectYPos, 6, 6, Matrices::matrixC.hasBegun ? 0xad55 : ST77XX_RED);
    rectXPos += 8;
    Display::baseDisplay.fillRect(rectXPos, rectYPos, 6, 6, Matrices::matrixD.hasBegun ? 0xad55 : ST77XX_RED);
}

void Display::drawConnection() {

    bool connected = Blesrv::isConnected();

    Display::baseDisplay.setTextSize(1);
    Display::baseDisplay.setTextColor(connected ? ST77XX_WHITE : 0x73ae);  // https://rgbcolorpicker.com/565

    uint16_t yPosC = 1;
    Display::drawString(Blesrv::macAdress, DISPLAY__WIDTH / 2, yPosC, TEXT_HALIGN_CENTER, 20, connected ? ST77XX_BLUE : 0x31a6);
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

    Display::baseDisplay.setTextSize(2);
    Display::baseDisplay.setTextColor(Orientation::hasBegun ? 0xad55 : 0xdc30);  // #adaaad : #de8684

    vector________t orientation = Orientation::getOrientation();

    uint8_t xPosValue = 24;

    uint8_t yPosOffset = DISPLAY_HEIGHT - 20;

    uint16_t yPosX = yPosOffset - 17 * 5;
    Display::drawString("X: ", 0, yPosX, TEXT_HALIGN___LEFT);
    Display::drawString(String(orientation.x, 2), xPosValue + getXLabelOffset(orientation.x), yPosX, TEXT_HALIGN___LEFT);

    uint16_t yPosY = yPosOffset - 17 * 4;
    Display::drawString("Y: ", 0, yPosY, TEXT_HALIGN___LEFT);
    Display::drawString(String(orientation.y, 2), xPosValue + getXLabelOffset(orientation.y), yPosY, TEXT_HALIGN___LEFT);

    uint16_t yPosZ = yPosOffset - 17 * 3;
    Display::drawString("Z: ", 0, yPosZ, TEXT_HALIGN___LEFT);
    Display::drawString(String(orientation.z, 2), xPosValue + getXLabelOffset(orientation.z), yPosZ, TEXT_HALIGN___LEFT);

    uint16_t yPosO = yPosOffset - 17 * 2;
    Display::drawString("O: ", 0, yPosO, TEXT_HALIGN___LEFT);
    Display::drawString(Device::getOrientation() == ORIENTATION______UP ? "UP  " : "DOWN", xPosValue, yPosO, TEXT_HALIGN___LEFT);
}

void Display::drawSignal() {

    // Serial.println(Microphone::signal);

    Display::baseDisplay.setTextSize(2);
    Display::baseDisplay.setTextColor(0xad55);  // #AAAAAA

    uint8_t barHeight = 70;
    uint8_t yPosOffset = DISPLAY_HEIGHT - 20 - 17;

    uint16_t signal = Microphone::signal * barHeight / 4096;

    uint16_t yPosO = yPosOffset - 17;  // same as o label
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
    Display::drawString(signalString, DISPLAY__WIDTH - 10, yPosO, TEXT_HALIGN__RIGHT);

    Display::baseDisplay.fillRect(DISPLAY__WIDTH - 3, yPosOffset - barHeight, 3, barHeight, ST77XX_BLACK);  // remove previous line
    Display::baseDisplay.fillRect(DISPLAY__WIDTH - 2, yPosOffset - barHeight, 1, barHeight, 0x632c);        // #666666
    Display::baseDisplay.fillRect(DISPLAY__WIDTH - 3, yPosOffset - signal, 3, signal, 0xad55);              // signal line
    Display::baseDisplay.drawFastHLine(DISPLAY__WIDTH - 8, yPosOffset - barHeight, 8, ST77XX_CYAN);
    Display::baseDisplay.drawFastHLine(DISPLAY__WIDTH - 8, yPosOffset - barHeight / 2, 8, ST77XX_CYAN);
    Display::baseDisplay.drawFastHLine(DISPLAY__WIDTH - 8, yPosOffset, 8, ST77XX_CYAN);
}