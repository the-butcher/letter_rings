#include "Matrices.h"

Matrix Matrices::matrixA(0x70, "A");
Matrix Matrices::matrixB(0x71, "B");
Matrix Matrices::matrixC(0x72, "C");
Matrix Matrices::matrixD(0x73, "D");

uint8_t Matrices::brightnessCurr = 64;
uint8_t Matrices::brightnessPend = 0;
bool Matrices::needsWrite = false;

bool Matrices::powerup() {
    if (!Matrices::matrixA.powerup()) {
        // Serial.println("failed to power up matrix A");
    }
    if (!Matrices::matrixB.powerup()) {
        // Serial.println("failed to power up matrix B");
    }
    if (!Matrices::matrixC.powerup()) {
        // Serial.println("failed to power up matrix C");
    }
    if (!Matrices::matrixD.powerup()) {
        // Serial.println("failed to power up matrix D");
    }
    return true;
}

bool Matrices::depower() {
    Matrices::matrixA.depower();
    Matrices::matrixB.depower();
    Matrices::matrixC.depower();
    Matrices::matrixD.depower();
    return true;
}

bool Matrices::setBrightness(uint8_t brightness) {
    if (brightness >= 0 && brightness <= 15) {
        Matrices::brightnessPend = brightness;
    }
    return Matrices::brightnessPend != Matrices::brightnessCurr;
}

uint8_t Matrices::getBrightness() {
    return Matrices::brightnessPend;
}

void Matrices::drawPixel(int16_t x, int16_t y, uint16_t color) {
    if (Device::getOrientation() == ORIENTATION______UP) {
        Matrices::matrixA.drawPixel(x, y, color);
        Matrices::matrixB.drawPixel(x - 8, y, color);
        Matrices::matrixC.drawPixel(x - 16, y, color);
        Matrices::matrixD.drawPixel(x - 24, y, color);
    } else {
        Matrices::matrixD.drawPixel(x, y, color);
        Matrices::matrixC.drawPixel(x - 8, y, color);
        Matrices::matrixB.drawPixel(x - 16, y, color);
        Matrices::matrixA.drawPixel(x - 24, y, color);
    }
    Matrices::needsWrite = true;
}

void Matrices::drawBars() {
    if (Device::getOrientation() == ORIENTATION______UP) {
        Matrices::matrixA.drawBars(0);
        Matrices::matrixB.drawBars(4);
        Matrices::matrixC.drawBars(8);
        Matrices::matrixD.drawBars(12);
    } else {
        Matrices::matrixD.drawBars(0);
        Matrices::matrixC.drawBars(4);
        Matrices::matrixB.drawBars(8);
        Matrices::matrixA.drawBars(12);
    }
    Matrices::needsWrite = true;
}

void Matrices::drawWord(String word) {
    if (Device::getOrientation() == ORIENTATION______UP) {
        Matrices::matrixA.drawWord(word, 0);
        Matrices::matrixB.drawWord(word, -8);
        Matrices::matrixC.drawWord(word, -16);
        Matrices::matrixD.drawWord(word, -24);
    } else {
        Matrices::matrixD.drawWord(word, 0);
        Matrices::matrixC.drawWord(word, -8);
        Matrices::matrixB.drawWord(word, -16);
        Matrices::matrixA.drawWord(word, -24);
    }
    Matrices::needsWrite = true;
}

void Matrices::drawLabel(String label, int16_t offset) {
    if (Device::getOrientation() == ORIENTATION______UP) {
        Matrices::matrixA.drawLabel(label, offset);
        Matrices::matrixB.drawLabel(label, offset - 8);
        Matrices::matrixC.drawLabel(label, offset - 16);
        Matrices::matrixD.drawLabel(label, offset - 24);
    } else {
        Matrices::matrixD.drawLabel(label, offset);
        Matrices::matrixC.drawLabel(label, offset - 8);
        Matrices::matrixB.drawLabel(label, offset - 16);
        Matrices::matrixA.drawLabel(label, offset - 24);
    }
    Matrices::needsWrite = true;
}

void Matrices::drawBitmapWithOrientation(const uint8_t* bitmap, int16_t offset, uint16_t color, orientation___e orientation) {

    orientation___e orientationDevice = Device::getOrientation();
    // if required, temporarily apply that orientation
    if (orientation != orientationDevice) {
        Matrices::setOrientation(orientation);
    }
    uint16_t sideOffset = BITMAPS_OFF * orientation * 16 + 16;  // either 0 or 32, depending on side and orientation
    if (orientation == ORIENTATION______UP) {
        Matrices::matrixA.drawBitmap(bitmap, offset - sideOffset, color, 8);
        Matrices::matrixB.drawBitmap(bitmap, offset - sideOffset - 8, color, 8);
        Matrices::matrixC.drawBitmap(bitmap, offset - sideOffset - 16, color, 8);
        Matrices::matrixD.drawBitmap(bitmap, offset - sideOffset - 24, color, 8);
    } else {
        Matrices::matrixD.drawBitmap(bitmap, offset - sideOffset, color, 8);
        Matrices::matrixC.drawBitmap(bitmap, offset - sideOffset - 8, color, 8);
        Matrices::matrixB.drawBitmap(bitmap, offset - sideOffset - 16, color, 8);
        Matrices::matrixA.drawBitmap(bitmap, offset - sideOffset - 24, color, 8);
    }

    // if required, return to device orientation
    if (orientation != orientationDevice) {
        Matrices::setOrientation(orientationDevice);
    }

    Matrices::needsWrite = true;
}

void Matrices::drawBitmap(const uint8_t* bitmap, int16_t offset, uint16_t color) {
    uint8_t width = sizeof(bitmap) * 8;
    if (Device::getOrientation() == ORIENTATION______UP) {
        Matrices::matrixA.drawBitmap(bitmap, offset, color, width);
        Matrices::matrixB.drawBitmap(bitmap, offset - 8, color, width);
        Matrices::matrixC.drawBitmap(bitmap, offset - 16, color, width);
        Matrices::matrixD.drawBitmap(bitmap, offset - 24, color, width);
    } else {
        Matrices::matrixD.drawBitmap(bitmap, offset, color, width);
        Matrices::matrixC.drawBitmap(bitmap, offset - 8, color, width);
        Matrices::matrixB.drawBitmap(bitmap, offset - 16, color, width);
        Matrices::matrixA.drawBitmap(bitmap, offset - 24, color, width);
    }
    Matrices::needsWrite = true;
}

void Matrices::clear() {
    Matrices::matrixA.clear();
    Matrices::matrixB.clear();
    Matrices::matrixC.clear();
    Matrices::matrixD.clear();
}

void Matrices::clearCanvases() {
    Matrices::matrixA.clearCanvases();
    Matrices::matrixB.clearCanvases();
    Matrices::matrixC.clearCanvases();
    Matrices::matrixD.clearCanvases();
}

bool Matrices::write() {
    bool written = false;
    if (Matrices::needsWrite) {
        Matrices::matrixA.write();
        Matrices::matrixB.write();
        Matrices::matrixC.write();
        Matrices::matrixD.write();
        Matrices::needsWrite = false;
        written = true;
    }
    if (Matrices::brightnessCurr != Matrices::brightnessPend) {
        Matrices::brightnessCurr = Matrices::brightnessPend;
        Matrices::matrixA.setBrightness(Matrices::brightnessCurr);
        Matrices::matrixB.setBrightness(Matrices::brightnessCurr);
        Matrices::matrixC.setBrightness(Matrices::brightnessCurr);
        Matrices::matrixD.setBrightness(Matrices::brightnessCurr);
        written = true;
    }
    return written;
}

void Matrices::setOrientation(orientation___e orientation) {
    Matrices::matrixA.setOrientation(orientation);
    Matrices::matrixB.setOrientation(orientation);
    Matrices::matrixC.setOrientation(orientation);
    Matrices::matrixD.setOrientation(orientation);
}

uint32_t* Matrices::getBuffer() {
    uint32_t* buffer32 = new uint32_t[8];
    uint8_t* bufferA = Matrices::matrixA.getBuffer();
    uint8_t* bufferB = Matrices::matrixB.getBuffer();
    uint8_t* bufferC = Matrices::matrixC.getBuffer();
    uint8_t* bufferD = Matrices::matrixD.getBuffer();
    if (Device::getOrientation() == ORIENTATION______UP) {
        for (uint8_t i = 0; i < 8; i++) {
            buffer32[i] = (bufferA[i] << 24) | (bufferB[i] << 16) | (bufferC[i] << 8) | (bufferD[i]);
        }
    } else {
        for (uint8_t i = 0; i < 8; i++) {
            buffer32[i] = (bufferD[i] << 24) | (bufferC[i] << 16) | (bufferB[i] << 8) | (bufferA[i]);
        }
    }
    return buffer32;
}
