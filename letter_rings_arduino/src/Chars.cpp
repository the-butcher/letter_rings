#include "Chars.h"


char__________t Chars::chars[CHARS______________NUM];

bool Chars::powerup() {

    char hannes[] = "HANNES";
    for (int i = 0; i < CHARS______________NUM; i++) {
        Chars::chars[i].character = hannes[i]; // (char)random(0x41, 0x5A);
        Chars::chars[i].position = {
            (float)random(0, CHARS______FIELD_DIM_X),
            (float)random(0, CHARS______FIELD_DIM_Y)
        };
        float direction = (float)(random(0, 100) * PI / 200.0);
        float velocity = (float)(random(100, 200) / 200.0);
        Chars::chars[i].velocity = {
            cos(direction) * velocity,
            sin(direction) * velocity
        };
    }

    return true;

}

void Chars::step() {

    for (int i = 0; i < CHARS______________NUM; i++) {
        Chars::chars[i].position = {
            Chars::chars[i].position.x + Chars::chars[i].velocity.x,
            Chars::chars[i].position.y + Chars::chars[i].velocity.y
        };

        uint64_t fieldDimNumX = (int)round(Chars::chars[i].position.x) / CHARS______FIELD_DIM_X;
        uint16_t fieldDimOffX = (int)round(Chars::chars[i].position.x) % CHARS______FIELD_DIM_X;
        if (fieldDimNumX % 2 == 0) {
            Chars::chars[i].matrixX = fieldDimOffX;
        } else {
            Chars::chars[i].matrixX = CHARS______FIELD_DIM_X - fieldDimOffX;
        }
        Chars::chars[i].matrixX += 13; // 16 - 3,
        Chars::chars[i].matrixX -= CHARS______FIELD_DIM_X / 2;

        uint64_t fieldDimNumY = (int)round(Chars::chars[i].position.y) / CHARS______FIELD_DIM_Y;
        uint16_t fieldDimOffY = (int)round(Chars::chars[i].position.y) % CHARS______FIELD_DIM_Y;
        if (fieldDimNumY % 2 == 0) {
            Chars::chars[i].matrixY = fieldDimOffY;
        } else {
            Chars::chars[i].matrixY = CHARS______FIELD_DIM_Y - fieldDimOffY;
        }
        Chars::chars[i].matrixY += 6;
        Chars::chars[i].matrixY -= CHARS______FIELD_DIM_Y / 2;

    }



}