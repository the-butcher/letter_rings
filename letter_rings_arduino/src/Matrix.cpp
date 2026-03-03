#include <Matrix.h>

Matrix::Matrix(uint8_t addr) : canvasA(8, 8), canvasB(8, 8) {
    this->baseMatrix = Adafruit_8x8matrix();
    this->addr = addr;
    this->hasBegun = false;
}

bool Matrix::powerup() {
    this->hasBegun = this->baseMatrix.begin(this->addr);  // pass in the address
    if (this->hasBegun) {
        this->baseMatrix.setRotation(2);
    }
    return this->hasBegun;
}

bool Matrix::depower() {
    // nothing
    return true;
}

void Matrix::setOrientation(orientation___e orientation) {
    if (this->hasBegun) {
        this->baseMatrix.setRotation(orientation == ORIENTATION______UP ? 2 : 0);
    }
}

uint16_t Matrix::getWordWidth(String word) {

    this->baseMatrix.setCursor(0, 0);
    this->baseMatrix.setFont(&Font7x8FixedMono);
    this->baseMatrix.setTextWrap(false);
    this->baseMatrix.setTextSize(1);

    int16_t x1, y1;
    uint16_t w, h;
    this->baseMatrix.getTextBounds(word, 0, 0, &x1, &y1, &w, &h);

    return w;
}

uint16_t Matrix::getLabelWidth(String label) {

    this->baseMatrix.setCursor(0, 0);
    this->baseMatrix.setFont();
    this->baseMatrix.setTextWrap(false);
    this->baseMatrix.setTextSize(1);

    int16_t x1, y1;
    uint16_t w, h;
    this->baseMatrix.getTextBounds(label, 0, 0, &x1, &y1, &w, &h);

    return w;
}

void Matrix::drawBars(uint8_t indexMin) {
    Matrix::clear();
    if (this->hasBegun) {
        double val1;
        double val2;
        double bar1;
        double bar2;
        for (uint8_t index = 0; index < 4; index++) {
            val1 = Microphone::lineValues[index + indexMin];
            val2 = Microphone::peakValues[index + indexMin];
            bar1 = index * 2;
            bar2 = index * 2 + 1;
            this->baseMatrix.drawFastVLine(bar1, 8 - val1, val1, LED_ON);
            this->baseMatrix.drawFastVLine(bar1, 8 - val2, 1, LED_ON);
            this->baseMatrix.drawFastVLine(bar2, 8 - val1, val1, LED_ON);
            this->baseMatrix.drawFastVLine(bar2, 8 - val2, 1, LED_ON);
        }
    }
}

void Matrix::drawWord(String word, int16_t offset) {
    Matrix::clear();
    if (this->hasBegun) {
        this->baseMatrix.setCursor(offset, 7);  // y-offset 7 is specifically for the 7x8 font
        this->baseMatrix.setFont(&Font7x8FixedMono);
        this->baseMatrix.setTextWrap(false);
        this->baseMatrix.setTextSize(1);
        this->baseMatrix.setTextColor(LED_ON);
        this->baseMatrix.print(word);
    }
}

void Matrix::drawLabel(String label, int16_t offset) {
    Matrix::clear();
    if (this->hasBegun) {
        this->baseMatrix.setCursor(offset, 0);  // y-offset 7 is specifically for the 7x8 font
        this->baseMatrix.setFont();
        this->baseMatrix.setTextWrap(false);
        this->baseMatrix.setTextSize(1);
        this->baseMatrix.setTextColor(LED_ON);
        this->baseMatrix.print(label);
    }
}

void Matrix::drawPixel(int16_t x, int16_t y, uint16_t color) {
    if (this->hasBegun) {
        this->baseMatrix.drawPixel(x, y, color);
    }
}

void Matrix::drawBitmap(const uint8_t* bitmap, int16_t offset, uint16_t color) {
    if (this->hasBegun) {
        this->baseMatrix.drawBitmap(offset, 0, bitmap, 8, 8, color);
    }
}

void Matrix::clear() {
    if (this->hasBegun) {
        this->baseMatrix.clear();
    }
}

void Matrix::write() {
    if (this->hasBegun) {
        this->baseMatrix.writeDisplay();
    }
}

void Matrix::setBrightness(uint8_t brightness) {
    if (this->hasBegun) {
        this->baseMatrix.setBrightness(brightness);
    }
}