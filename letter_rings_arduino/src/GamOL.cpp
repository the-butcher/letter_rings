#include "GamOL.h"

uint32_t GamOL::fieldState[8];
uint8_t GamOL::fieldCount[8][32];

void GamOL::readFieldState() {

    uint32_t* buffer = Matrices::getBuffer();
    for (uint8_t i = 0; i < 8; i++) {
        GamOL::fieldState[i] = buffer[i];
    }

    // String formattedNumber;
    // for (uint8_t i = 0; i < 8; i++) {
    //     formattedNumber = String(GamOL::fieldState[i], 2);
    //     for (uint8_t p = 0; p < 32 - formattedNumber.length(); p++) {
    //         Serial.print("0");
    //     }
    //     Serial.println(formattedNumber);
    // }

}

void GamOL::stepFieldState() {

    // reset previous counts
    for (uint8_t y = 0; y < 8; y++) { // each row
        for (uint8_t x = 0; x < 32; x++) { // each column
            GamOL::fieldCount[y][x] = 0;
        }
    }

    // count values
    uint8_t value;
    for (uint8_t y = 0; y < 8; y++) { // each row
        for (uint8_t x = 0; x < 32; x++) { // each column
            value = bitRead(GamOL::fieldState[y], x);
            if (value > 0) {
                GamOL::increaseCount(x - 1, y - 1);
                GamOL::increaseCount(x, y - 1);
                GamOL::increaseCount(x + 1, y - 1);
                GamOL::increaseCount(x - 1, y);
                // here is the cell itself
                GamOL::increaseCount(x + 1, y);
                GamOL::increaseCount(x - 1, y + 1);
                GamOL::increaseCount(x, y + 1);
                GamOL::increaseCount(x + 1, y + 1);
            }
        }
    }

    // one generation forward
    uint8_t count;
    for (uint8_t y = 0; y < 8; y++) { // each row
        for (uint8_t x = 0; x < 32; x++) { // each value
            value = bitRead(GamOL::fieldState[y], x);
            count = GamOL::fieldCount[y][x];
            if (value > 0) { // cell is currently alive
                if (count < 2 || count > 3) { // cell dies
                    GamOL::fieldState[y] = bitWrite(GamOL::fieldState[y], x, 0);
                }
            } else {
                if (count == 3) { // cell spawns
                    GamOL::fieldState[y] = bitWrite(GamOL::fieldState[y], x, 1);
                }
            }
        }
    }

}

void GamOL::increaseCount(uint8_t x, uint8_t y) {
    GamOL::fieldCount[(y + 8) % 8][(x + 32) % 32]++;
}

void GamOL::drawFieldState() {

    uint8_t* bitmap = new uint8_t[32];
    for (uint8_t i = 0; i < 8; i++) { // each row
        bitmap[i * 4 + 0] = (GamOL::fieldState[i] >> 24) & 0xff;
        bitmap[i * 4 + 1] = (GamOL::fieldState[i] >> 16) & 0xff;
        bitmap[i * 4 + 2] = (GamOL::fieldState[i] >> 8) & 0xff;
        bitmap[i * 4 + 3] = (GamOL::fieldState[i] >> 0) & 0xff;
    }

    // String formattedNumber;
    // for (uint8_t i = 0; i < 32; i++) { // each row
    //     formattedNumber = String(bitmap[i], 2);
    //     for (uint8_t p = 0; p < 8 - formattedNumber.length(); p++) {
    //         Serial.print("0");
    //     }
    //     Serial.print(formattedNumber);
    //     if (i > 0 && (i + 1) % 4 == 0) {
    //         Serial.println();
    //     } else {
    //         Serial.print("|");
    //     }
    // }
    // Serial.println();

    Matrices::clear();
    Matrices::clearCanvases();
    Matrices::drawBitmap(bitmap, 0, LED_ON);

}