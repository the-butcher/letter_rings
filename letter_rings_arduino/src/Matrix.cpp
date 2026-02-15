#include <Matrix.h>

// Adafruit_8x8matrix Matrix::baseMatrix();

Matrix::Matrix(uint8_t addr) : canvasA(8, 8), canvasB(8, 8) {
    this->baseMatrix = Adafruit_8x8matrix();
    this->addr = addr;
    this->hasBegun = false;

    // initialize before scrambling
    // for (uint8_t i = 0; i < 64; i++) {
    //     this->copyOrder[i] = i;
    // }

    // Start from the last element and swap one by one. We don't
    // need to run for the first element that's why i > 0
    // for (int i = 63; i > 0; i--) {
    //     long j = random(0, 64);
    //     int temp = this->copyOrder[i];
    //     this->copyOrder[i] = this->copyOrder[j];
    //     this->copyOrder[j] = temp;
    // }
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

    // Serial.print(x1);
    // Serial.print(", ");
    // Serial.print(y1);
    // Serial.print(", ");
    // Serial.print(w);
    // Serial.print(", ");
    // Serial.println(h);

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
    if (this->hasBegun) {
        this->baseMatrix.clear();

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

        this->baseMatrix.writeDisplay();
    }
}

void Matrix::drawWord(String word, int16_t offset) {
    if (this->hasBegun) {
        this->baseMatrix.clear();
        this->baseMatrix.setCursor(offset, 7);  // y-offset 7 is specifically for the 7x8 font
        this->baseMatrix.setFont(&Font7x8FixedMono);
        this->baseMatrix.setTextWrap(false);
        this->baseMatrix.setTextSize(1);
        this->baseMatrix.setTextColor(LED_ON);
        this->baseMatrix.print(word);
        this->baseMatrix.writeDisplay();
    }
}

void Matrix::drawLabel(String label, int16_t offset) {
    if (this->hasBegun) {
        this->baseMatrix.clear();
        this->baseMatrix.setCursor(offset, 0);  // y-offset 7 is specifically for the 7x8 font
        this->baseMatrix.setFont();
        this->baseMatrix.setTextWrap(false);
        this->baseMatrix.setTextSize(1);
        this->baseMatrix.setTextColor(LED_ON);
        this->baseMatrix.print(label);
        this->baseMatrix.writeDisplay();
    }
}

// void Matrix::drawTextToCanvasA(String text, int16_t offset) {
//     if (this->hasBegun) {
//         this->canvasA.fillRect(0, 0, 8, 8, LED_OFF);
//         this->canvasA.setCursor(offset, 7);  // y-offset 7 is specifically for the 7x8 font
//         this->canvasA.setFont(&Font7x8FixedMono);
//         this->canvasA.setTextWrap(false);
//         this->canvasA.setTextSize(1);
//         this->canvasA.setTextColor(LED_ON);
//         this->canvasA.print(text);
//         // uint8_t* bitmapA = this->canvasA.getBuffer();
//         // for (uint8_t i = 0; i < 8; i++) {
//         //     Serial.println(String(bitmapA[i], 2));
//         // }
//     }
// }

// void Matrix::copyCanvasAtoCanvasB(uint8_t progress, bool skipPrevious) {
//     // Serial.println(progress);
//     // uint8_t* bitmapA = this->canvasA.getBuffer();
//     // for (uint8_t i = 0; i < 8; i++) {
//     //     Serial.println(String(bitmapA[i], 2));
//     // }
//     // Serial.println("-----------------");
//     uint8_t byteIndex;
//     uint8_t bitIndex;
//     uint8_t* bitmapB = this->canvasB.getBuffer();
//     uint8_t byteA;
//     uint8_t byteB;
//     for (uint8_t i = skipPrevious ? progress - 1 : 0; i < progress; i++) {
//         byteIndex = this->copyOrder[i] / 8;
//         bitIndex = this->copyOrder[i] % 8;
//         byteA = this->canvasA.getBuffer()[byteIndex];
//         // Serial.println(String(byteA, 2));
//         byteB = bitmapB[byteIndex];
//         bitWrite(byteB, bitIndex, bitRead(byteA, bitIndex));
//         bitmapB[byteIndex] = byteB;
//     }
//     // Serial.println("-----------------");
//     // for (uint8_t i = 0; i < 8; i++) {
//     //     Serial.println(String(bitmapB[i], 2));
//     // }
//     // Serial.println("-----------------");
//     this->canvasB.drawBitmap(0, 0, bitmapB, 8, 8, LED_ON);
//     // TODO :: copy bytes as defined in transition from cnavasA to canvasB
// }

// void Matrix::flushCanvasBtoMatrix() {
//     this->drawBitmap(this->canvasB.getBuffer());
// }

void Matrix::drawBitmap(const uint8_t* bitmap) {
    if (this->hasBegun) {
        this->baseMatrix.clear();
        this->baseMatrix.drawBitmap(0, 0, bitmap, 8, 8, LED_ON);
        this->baseMatrix.writeDisplay();
    }
}

void Matrix::setBrightness(uint8_t brightness) {
    if (this->hasBegun) {
        this->baseMatrix.setBrightness(brightness);
    }
}