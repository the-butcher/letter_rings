#include <Arduino.h>

#include "Blesrv.h"
#include "Buttons.h"
#include "Device.h"
#include "Display.h"
#include "Matrices.h"
#include "Microphone.h"
#include "Nowsrv.h"
#include "Orientation.h"

uint64_t counter = 0;

int16_t labelPos = 33;
String mainLabel = "";
uint16_t labelWidth = 32;  // start value, will be recalculated upon first text change

String mainWord = "";
uint8_t wordProgress = 0;
const uint8_t WORD_TRANSITION_COUNT = 10;
const uint64_t WORD_UPDATE_INTERVAL_MS = 14000;

const uint64_t PARTY_LABEL_DURATION_MS = 20000;

uint64_t lastWordUpdateMillis = 0;
uint64_t lastLabelUpdateMillis = 0;
uint64_t lastOrientationUpdateMillis = 0;

int16_t bitmapPos = -8;

/**
 * power up orientation, then reads new values every 10 milliseconds
 */
void orientationBegin(void* pvParameters) {

    Orientation::powerup();
    delay(100);

    // const TickType_t delayMs = 25 / portTICK_PERIOD_MS;
    // Serial.print("delayMs: ");
    // Serial.println(delayMs);

    uint32_t millisA;
    uint32_t millisB;
    while (true) {
        millisA = millis();
        Orientation::readval();
        millisB = millis();
        vTaskDelay(25 - (millisB - millisA));
    }

    vTaskDelete(NULL);
}

void setup(void) {

    Serial.begin(9600);
    delay(2000);

    // https// forum.arduino.cc/t/shuffle-an-array-of-ints/333494/6
    // use a different seed value so that we don't get same
    // result each time we run this program
    randomSeed(analogRead(A0));

    Serial.print("setup ");
    Serial.print(BLE_DEVICE_NAME);
    Serial.println(" ...");

    Display::powerup();
    delay(100);

    Blesrv::begin();
    delay(100);

    Matrices::powerup();
    delay(100);

    Microphone::powerup();
    delay(100);

    Buttons::powerup();
    delay(100);

    Nowsrv::begin();
    delay(100);

    xTaskCreatePinnedToCore(orientationBegin, "orientation-begin", 100000, NULL, 1, NULL, 0);

    Serial.println("... setup");
}

bool exceedsWordUpdateInterval(uint64_t currMillis) {
    return (currMillis - lastWordUpdateMillis) > WORD_UPDATE_INTERVAL_MS;
}

bool exceedsPartyLabelDuration(uint64_t currMillis) {
    return (currMillis - lastLabelUpdateMillis) > PARTY_LABEL_DURATION_MS;
}

void loop() {

    uint64_t currMillis = millis();

    // label needs to be checks outside of MODUS________LABEL, so a correct decision can be made when in MODUS________PARTY
    if (Device::label != mainLabel) {  // width needs to be calculated
        labelWidth = Matrices::matrixA.getLabelWidth(Device::label);
        mainLabel = Device::label;
        lastLabelUpdateMillis = currMillis;
        // Serial.print("labelWidth: ");
        // Serial.println(String(labelWidth));
        // Serial.print("label: ");
        // Serial.println(String(mainLabel));
    }

    // if in party mode, make a decision for either label or frequency
    modus_________e modus = Device::getCurrModus();
    if (modus == MODUS________ACCEL) {
        if (Device::getDeviceRole() == DEVICE_ROLE_____ANY) {  // (not primary, not secondary) = below coefficient threshold
            modus = Device::getPrevModus();
            Serial.print("modus accel, falling back to prev: ");
            Serial.println(modus);
        } else {
            Serial.println("modus accel, keeping due to pri or sec role");
        }
    }

    if (modus == MODUS________PARTY) {
        modus = exceedsPartyLabelDuration(currMillis) ? MODUS________FREQU : MODUS________LABEL;
    }

    if (Display::needsStatusRedraw) {
        // Serial.println("drawStatus");
        Display::drawStatus(modus);
    }

    if (Matrices::needsBrightnessUpdate) {
        Matrices::updateBrightness();
    }

    // update position (max update count/second = 10)
    if ((currMillis - lastOrientationUpdateMillis) > 100) {

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

        if (Device::getCurrModus() == MODUS________ACCEL) {  // must refer to actual curr device modus
            Display::drawAcceleration();
            Nowsrv::sendAcceleration();  // consumes ~1 millisecond
            Nowsrv::sendBitmaps(Device::getSendBitmaps());
        }

        if (modus == MODUS________FREQU) {
            Display::drawFrequ();
        }

        Display::drawDeviceRole();

        // uint64_t millisB = millis();
        // Serial.println(String(millisB - millisA));

        lastOrientationUpdateMillis = currMillis;
    }

    if (modus == MODUS________WORDS) {

        // if ((currMillis - lastWordUpdateMillis) > WORD_UPDATE_INTERVAL_MS) {
        if (exceedsWordUpdateInterval(currMillis)) {
            uint64_t randomWordIndex = random(0, WORD_COUNT - 1);
            // Serial.print("randomWordIndex: ");
            // Serial.print(String(randomWordIndex));
            // Serial.print(", word: ");
            // Serial.println(WORDS[randomWordIndex]);
            Device::word = WORDS[randomWordIndex];  // will be picked up some lines down
        }
        if (Device::word != mainWord) {
            mainWord = Device::word;
            wordProgress = 0;
            lastWordUpdateMillis = currMillis;
        }

        if (wordProgress < WORD_TRANSITION_COUNT) {
            wordProgress++;
            String word = WORDS[random(0, WORD_COUNT - 1)];
            Display::drawText(word);
            Matrices::drawWord(word);
            delay(50);
        } else {
            if (wordProgress == WORD_TRANSITION_COUNT) {
                wordProgress++;
                Display::drawText(mainWord);
                Matrices::drawWord(mainWord);
            }
            delay(100);
        }

    } else if (modus == MODUS________LABEL) {

        Matrices::drawLabel(mainLabel, labelPos);

        if (labelPos < -labelWidth) {  // all the way out to the left
            labelPos = 32;             // reset to right border
        }
        labelPos--;

        // if (Orientation::getOrientation().z * Device::getOrientation() < -25) {
        //     if (labelPos < -labelWidth) {  // all the way out to the left
        //         labelPos = 32;           // reset to right border
        //     }
        //     labelPos--;
        // } else if (Orientation::getOrientation().z * Device::getOrientation() > 25) {
        //     if (labelPos > 32) {       // all the way out to the right
        //         labelPos = -labelWidth;  // reset to left border
        //     }
        //     labelPos++;
        // }
        delay(max(50.0, 150 - Orientation::getOrientation().z));

    } else if (modus == MODUS________FREQU) {  // frequency

        Microphone::sample();
        Matrices::drawBars();

        delay(1);

    } else if (modus == MODUS________ACCEL) {

        Matrices::clear();

        bitmaps_______t bitmaps = Device::currBitmaps;
        bitmaps.bitmapA.bitmap = (bitmap________e)(bitmapPos % 2);  // mouth open or closed

        Matrices::drawBitmap(BITMAP_STORE[bitmaps.bitmapA.bitmap], bitmapPos + bitmaps.bitmapA.offset);
        Matrices::drawBitmap(BITMAP_STORE[bitmaps.bitmapB.bitmap], bitmapPos + bitmaps.bitmapB.offset);

        if (bitmapPos > 64) {
            bitmapPos = -8;
        }
        bitmapPos++;

        delay(100);

    } else {
        // TODO :: warn about unknown modus on display
        Serial.print("unknown modus: ");
        Serial.println(String(modus));
        delay(100);
    }

    counter++;
}
