#include <Matrix.h>

Matrix::Matrix(uint8_t addr, String name) : writeCanvas(8, 8), clearCanvas(8, 8) {
    this->baseMatrix = Adafruit_8x8matrix();
    this->addr = addr;
    this->name = name;
    this->brightnessCurr = 64;
    this->brightnessPend = 0;
    this->powered = false;
}

bool Matrix::powerup() {
    this->powered = this->baseMatrix.begin(this->addr);  // pass in the address
    if (this->powered) {
        this->baseMatrix.setRotation(2);
    }
    return this->powered;
}

bool Matrix::depower() {
    // nothing
    return true;
}

void Matrix::setOrientation(orientation___e orientation) {
    if (this->powered) {
        this->baseMatrix.setRotation(orientation == ORIENTATION______UP ? 2 : 0);
    }
}

uint16_t Matrix::getWordWidth(String word) {

    this->writeCanvas.setCursor(0, 0);
    this->writeCanvas.setFont(&Font7x8FixedMono);
    this->writeCanvas.setTextWrap(false);
    this->writeCanvas.setTextSize(1);

    int16_t x1, y1;
    uint16_t w, h;
    this->writeCanvas.getTextBounds(word, 0, 0, &x1, &y1, &w, &h);

    return w;
}

uint16_t Matrix::getLabelWidth(String label) {

    this->writeCanvas.setCursor(0, 0);
    this->writeCanvas.setFont();
    this->writeCanvas.setTextWrap(false);
    this->writeCanvas.setTextSize(1);

    int16_t x1, y1;
    uint16_t w, h;
    this->writeCanvas.getTextBounds(label, 0, 0, &x1, &y1, &w, &h);

    return w;
}

void Matrix::drawBars(uint8_t indexMin) {
    this->clear(CLEAR_MATRIX_CANVAS | CLEAR_MATRIX___DISP);
    if (this->powered) {
        double val1;
        double val2;
        double bar1;
        double bar2;
        for (uint8_t index = 0; index < 4; index++) {
            val1 = Microphone::lineValues[index + indexMin];
            val2 = Microphone::peakValues[index + indexMin];
            bar1 = index * 2;
            bar2 = index * 2 + 1;
            this->writeCanvas.drawFastVLine(bar1, 8 - val1, val1, LED_ON);
            this->writeCanvas.drawFastVLine(bar1, 8 - val2, 1, LED_ON);
            this->writeCanvas.drawFastVLine(bar2, 8 - val1, val1, LED_ON);
            this->writeCanvas.drawFastVLine(bar2, 8 - val2, 1, LED_ON);
        }
    }
}

void Matrix::drawChar(char__________t character, int16_t offset) {
    // no clearing here, must be done centrally
    if (this->powered) {
        this->writeCanvas.setCursor(offset + character.matrixX, character.matrixY);
        this->writeCanvas.setFont(&Font5x6FixedMono);
        this->writeCanvas.setTextWrap(false);
        this->writeCanvas.setTextSize(1);
        this->writeCanvas.setTextColor(LED_ON);
        this->writeCanvas.print(String(character.character));
    }
}

void Matrix::drawWord(String word, int16_t offset) {
    this->clear(CLEAR_MATRIX_CANVAS | CLEAR_MATRIX___DISP);
    if (this->powered) {
        this->writeCanvas.setCursor(offset, 7);  // y-offset 7 is specifically for the 7x8 font
        this->writeCanvas.setFont(&Font7x8FixedMono);
        this->writeCanvas.setTextWrap(false);
        this->writeCanvas.setTextSize(1);
        this->writeCanvas.setTextColor(LED_ON);
        this->writeCanvas.print(word);
    }
}

void Matrix::drawLabel(String label, int16_t offset) {
    this->clear(CLEAR_MATRIX_CANVAS | CLEAR_MATRIX___DISP);
    if (this->powered) {
        this->writeCanvas.setCursor(offset, 0);  // y-offset 7 is specifically for the 7x8 font
        this->writeCanvas.setFont();
        this->writeCanvas.setTextWrap(false);
        this->writeCanvas.setTextSize(1);
        this->writeCanvas.setTextColor(LED_ON);
        this->writeCanvas.print(label);
    }
}

void Matrix::drawLedbar(uint8_t value) {
    this->clear(CLEAR_MATRIX_CANVAS | CLEAR_MATRIX___DISP);
    if (this->powered) {
        Matrix::setBrightness(value);
        // if (value > 9) {
        //     this->writeCanvas.drawRect(1, 1, 6, 6, LED_ON);
        // } else if (value > 6) {
        this->writeCanvas.drawRect(2, 2, 4, 4, LED_ON);
        // } else {
        //     this->writeCanvas.drawRect(3, 3, 2, 2, LED_ON); // always draw innermost
        // }
    }
}

void Matrix::drawPixel(int16_t x, int16_t y, uint16_t color) {
    if (this->powered) {
        this->writeCanvas.drawPixel(x, y, color);
    }
}

void Matrix::drawBitmap(const uint8_t* bitmap, int16_t offset, uint16_t color, uint8_t width) {
    if (this->powered) {
        if (color == LED_ON) {
            this->writeCanvas.drawBitmap(offset, 0, bitmap, width, 8, LED_ON);
        } else {
            this->clearCanvas.drawBitmap(offset, 0, bitmap, width, 8, LED_ON);
        }
    }
}

void Matrix::clear(uint8_t flags) {
    if (this->powered) {
        if ((flags & CLEAR_MATRIX_CANVAS) != 0) {
            this->clearCanvas.fillRect(0, 0, 8, 8, LED_OFF);
            this->writeCanvas.fillRect(0, 0, 8, 8, LED_OFF);
        }
        if ((flags & CLEAR_MATRIX___DISP) != 0) {
            this->baseMatrix.clear();
        }
    }
}

void Matrix::write() {
    if (this->powered) {
        this->baseMatrix.drawBitmap(0, 0, this->clearCanvas.getBuffer(), 8, 8, LED_OFF);
        this->baseMatrix.drawBitmap(0, 0, this->writeCanvas.getBuffer(), 8, 8, LED_ON);
        this->baseMatrix.writeDisplay();
        if (this->brightnessCurr != this->brightnessPend) {
            this->brightnessCurr = this->brightnessPend;
            this->baseMatrix.setBrightness(this->brightnessCurr);
        }
    }
}

bool Matrix::setBrightness(uint8_t brightness) {
    if (brightness >= 0 && brightness <= 15) {
        this->brightnessPend = brightness;
    }
    return this->brightnessPend != this->brightnessCurr;
}

uint8_t* Matrix::getBuffer() {
    return this->writeCanvas.getBuffer();
}