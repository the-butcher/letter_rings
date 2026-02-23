#include "Matrices.h"

Matrix Matrices::matrixA(0x70);
Matrix Matrices::matrixB(0x71);
Matrix Matrices::matrixC(0x72);
Matrix Matrices::matrixD(0x73);

uint8_t Matrices::brightness = 0;
bool Matrices::needsBrightnessUpdate = true;

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
    // Matrices::canvas.getBuffer();
    return true;
}

bool Matrices::depower() {
    Matrices::matrixA.depower();
    Matrices::matrixB.depower();
    Matrices::matrixC.depower();
    Matrices::matrixD.depower();
    return true;
}

void Matrices::updateBrightness() {
    Matrices::matrixA.setBrightness(Matrices::brightness);
    Matrices::matrixB.setBrightness(Matrices::brightness);
    Matrices::matrixC.setBrightness(Matrices::brightness);
    Matrices::matrixD.setBrightness(Matrices::brightness);
    Matrices::needsBrightnessUpdate = false;
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
}

// void Matrices::drawTextToCanvasA(String text) {
//     if (Matrices::orientation == ORIENTATION______UP) {
//         Matrices::matrixA.drawTextToCanvasA(text, 0);
//         Matrices::matrixB.drawTextToCanvasA(text, -8);
//         Matrices::matrixC.drawTextToCanvasA(text, -16);
//         Matrices::matrixD.drawTextToCanvasA(text, -24);
//     } else {
//         Matrices::matrixD.drawTextToCanvasA(text, 0);
//         Matrices::matrixC.drawTextToCanvasA(text, -8);
//         Matrices::matrixB.drawTextToCanvasA(text, -16);
//         Matrices::matrixA.drawTextToCanvasA(text, -24);
//     }
// }

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
}

void Matrices::clear() {
    Matrices::matrixA.clear();
    Matrices::matrixB.clear();
    Matrices::matrixC.clear();
    Matrices::matrixD.clear();
}

void Matrices::write() {
    Matrices::matrixA.write();
    Matrices::matrixB.write();
    Matrices::matrixC.write();
    Matrices::matrixD.write();
}

void Matrices::drawBitmap(const uint8_t* bitmap, int16_t offset, uint16_t color) {
    // TODO :: account for increased width when paired
    // TODO :: account for UP/DOWN
    Matrices::matrixA.drawBitmap(bitmap, offset, color);
    Matrices::matrixB.drawBitmap(bitmap, offset - 8, color);
    Matrices::matrixC.drawBitmap(bitmap, offset - 16, color);
    Matrices::matrixD.drawBitmap(bitmap, offset - 24, color);
}

// void Matrices::copyCanvasAtoCanvasB(uint8_t progress, bool skipPrevious) {
//     Matrices::matrixA.copyCanvasAtoCanvasB(progress, skipPrevious);
//     Matrices::matrixB.copyCanvasAtoCanvasB(progress, skipPrevious);
//     Matrices::matrixC.copyCanvasAtoCanvasB(progress, skipPrevious);
//     Matrices::matrixD.copyCanvasAtoCanvasB(progress, skipPrevious);
// }

// void Matrices::flushCanvasBtoMatrix() {
//     Matrices::matrixA.flushCanvasBtoMatrix();
//     Matrices::matrixB.flushCanvasBtoMatrix();
//     Matrices::matrixC.flushCanvasBtoMatrix();
//     Matrices::matrixD.flushCanvasBtoMatrix();
// }