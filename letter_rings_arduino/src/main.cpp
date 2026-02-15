#include <Arduino.h>

#include "Blesrv.h"
#include "Buttons.h"
#include "Device.h"
#include "Display.h"
#include "Matrices.h"
#include "Microphone.h"
#include "Orientation.h"

uint64_t counter = 0;
int16_t basePos = 33;
String label = "";
uint16_t runWidth = 32;  // start value, will be recalculated upon first text change

uint8_t wordProgress = 0;
uint64_t lastWordUpdateMillis = 0;
uint64_t wordIndex = 0;

uint64_t lastOrientationUpdateMillis = 0;

void setup(void) {

    Serial.begin(9600);
    delay(2000);

    // https// forum.arduino.cc/t/shuffle-an-array-of-ints/333494/6
    // use a different seed value so that we don't get same
    // result each time we run this program
    randomSeed(analogRead(A0));

    Serial.println("setup ...");

    Display::powerup();
    delay(100);

    Blesrv::begin();
    delay(100);

    Matrices::powerup();
    delay(100);

    Microphone::powerup();
    delay(100);

    Orientation::powerup();
    delay(100);

    Buttons::powerup();
    delay(100);

    Serial.println("... setup");
}

void loop() {

    uint64_t currMillis = millis();

    if (Display::needsStatusRedraw) {
        Serial.println("drawStatus");
        Display::drawStatus();
    }

    if (Matrices::needsBrightnessUpdate) {
        Matrices::updateBrightness();
    }

    // update position (max update count/second = 10)
    if ((currMillis - lastOrientationUpdateMillis) > 100) {
        Orientation::readval();
        orientation___e matricesOrientation = Device::getOrientation();
        if (matricesOrientation == ORIENTATION______UP && Orientation::getOrientation().y > 20) {
            Device::setOrientation(ORIENTATION____DOWN);
        }
        if (matricesOrientation == ORIENTATION____DOWN && Orientation::getOrientation().y < -20) {
            Device::setOrientation(ORIENTATION______UP);
        }
        if (Orientation::hasBegun) {
            Display::drawOrientation();
        }
        Display::drawSignal();

        if (Device::modus == MODUS________FREQU) {
            Display::drawFrequ();
        }

        lastOrientationUpdateMillis = currMillis;
    }

    if (Device::modus == MODUS________WORDS) {

        if ((currMillis - lastWordUpdateMillis) > 10000) {
            wordProgress = 0;
            lastWordUpdateMillis = currMillis;
        }

        if (wordProgress < 10) {
            wordProgress++;
            String word = WORDS[random(0, 119)];
            Display::drawText(word);
            Matrices::drawWord(word);
            delay(50);
        } else {
            delay(100);
        }

    } else if (Device::modus == MODUS________LABEL) {

        if (Matrices::label != label) {  // width needs to be calculated
            runWidth = Matrices::matrixA.getLabelWidth(Matrices::label);
            label = Matrices::label;
            Serial.print("runwidth: ");
            Serial.println(String(runWidth));
            Serial.print("label: ");
            Serial.println(String(label));
        }

        Matrices::drawLabel(label, basePos);

        if (Orientation::getOrientation().z * Device::getOrientation() < -25) {
            if (basePos < -runWidth) {  // all the way out to the left
                basePos = 32;           // reset to right border
            }
            basePos--;
        } else if (Orientation::getOrientation().z * Device::getOrientation() > 25) {
            if (basePos > 32) {       // all the way out to the right
                basePos = -runWidth;  // reset to left border
            }
            basePos++;
        }
        delay(150 - abs(Orientation::getOrientation().z));

    } else {  // frequency

        Microphone::sample();
        Matrices::drawBars();

        delay(1);
    }

    counter++;
}
