#include <Arduino.h>

#include "Blesrv.h"
#include "Buttons.h"
#include "Device.h"
#include "Display.h"
#include "Matrices.h"
#include "Microphone.h"
#include "Nowsrv.h"
#include "Orientation.h"

uint16_t pixelPos = 0;
int16_t labelPos = 33;
String mainLabel = "";
uint16_t labelWidth = 32;  // start value, will be recalculated upon first text change

String mainWord = "";
uint8_t wordProgress = 0;
const uint8_t WORD_TRANSITION_COUNT = 15;

uint64_t lastWordUpdateMillis = 0;
uint64_t lastLabelUpdateMillis = 0;
uint64_t lastGeneralUpdateMillis = 0;
uint64_t lastAccelerationSendMillis = 0;

const int16_t BITMAP_RESET_POS = -10;
int16_t bitmapPos = BITMAP_RESET_POS;

uint64_t millisAPri = 0;
uint64_t durationAB = 0;
uint64_t durationAC = 0;

#if USE_SERIAL_LOOP_OUTPUT == true
uint64_t totalLoopMillis = 0;
uint64_t totalLoopNumber = 0;
#endif

bool exceedsWordUpdateInterval(uint64_t currMillis) {
    return (currMillis - lastWordUpdateMillis) > WORD_UPDATE_INTERVAL_MS;
}

bool exceedsPartyLabelDuration(uint64_t currMillis) {
    return (currMillis - lastLabelUpdateMillis) > PARTY_LABEL_DURATION_MS;
}

modus_________e determineModus() {

    // label needs to be checked outside of MODUS________LABEL clause, so a correct decision can be made when in MODUS________PARTY
    if (Device::label != mainLabel) {
        labelWidth = Matrices::matrixA.getLabelWidth(Device::label);
        mainLabel = Device::label;
        lastLabelUpdateMillis = millis();
    }

    modus_________e modus = Device::getCurrModus();
    // if in accel mode and paired -> run pacman, fallback to previous mode otherwise
    if (modus == MODUS________ACCEL) {
        if (Device::getDeviceRole() == DEVICE_ROLE_____ANY && !USE__FORCE_MODUS_ACCEL) {  // (not primary, not secondary) = below coefficient threshold
            modus = Device::getPrevModus();
        }
    }

    // if in party mode, make a decision for either label or frequency
    if (modus == MODUS________PARTY) {
        modus = exceedsPartyLabelDuration(millis()) ? MODUS________FREQU : MODUS________LABEL;
    }

    return modus;
}

/**
 * run anything that will lead to matrix or display updates
 */
void runLoopTaskDisplay(void* pvParameters) {

    while (true) {

        uint64_t currMillis = millis();

        modus_________e modus = determineModus();

        orientation___e matricesOrientation = Device::getOrientation();
        if (matricesOrientation == ORIENTATION______UP && Orientation::getOrientation().y > 20) {
            Device::setOrientation(ORIENTATION____DOWN);
        }
        if (matricesOrientation == ORIENTATION____DOWN && Orientation::getOrientation().y < -20) {
            Device::setOrientation(ORIENTATION______UP);
        }

        // common display redraws (have to be in this loop to keep the clip logic upright)
        // this will happen at varying intervals, however all of them in the same magnitude
        Display::drawConfig(); // only draws when the needsConfigRedraw is true
        Display::drawConnection(); // only draws when the connection state changed
        Display::drawDeviceRole();  // little box indicators
        Display::drawOrientation(); // will only draw if orientation was properly initialized
        Display::drawSignal(); // will always draw
        Display::drawMatrixState();  // I2C init states of matrices, only draws once

        if (modus == MODUS________WORDS) {

            if (exceedsWordUpdateInterval(currMillis)) {
                uint64_t randomWordIndex = random(0, WORD_COUNT - 1);
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
                Matrices::drawWord(word);
                Display::drawText(word);
                vTaskDelay(75);
            } else {
                if (wordProgress == WORD_TRANSITION_COUNT) {
                    wordProgress++;
                    Matrices::drawWord(mainWord);
                    Display::drawText(mainWord);
                }
                vTaskDelay(100);
            }

        } else if (modus == MODUS________LABEL) {

            Matrices::drawLabel(mainLabel, labelPos);
            Display::drawText(mainLabel);

            if (labelPos < -labelWidth) {  // all the way out to the left
                labelPos = 32;             // reset to right border
            }
            labelPos--;

            vTaskDelay(max(50.0, 150 - Orientation::getOrientation().z));

        } else if (modus == MODUS________FREQU) {  // frequency

            // Matrices are draw in primary loop
            Display::drawFrequ();

            vTaskDelay(100);  // effectively do nothing, just wait a bit

        } else if (modus == MODUS________BREAK) {  // frequency

            // TODO :: proceed a single dot
            Matrices::clear();
            Matrices::drawPixel(pixelPos, 6, LED_ON);

            Display::drawText("");

            if (pixelPos > 31) {  // all the way out to the left
                pixelPos = 0;             // reset to right border
            }
            pixelPos++;

            vTaskDelay(250);

        } else if (modus == MODUS________ACCEL) {

            bitmaps_______t bitmaps = Device::currBitmaps;

            // first two calls clear the bitmap ahead and behind
            Matrices::drawBitmap(BITMAP_STORE[bitmaps.bitmapA.bitmap], bitmaps.bitmapB.offset + 2, bitmaps.bitmapA.color, bitmaps.orientation);
            Matrices::drawBitmap(BITMAP_STORE[bitmaps.bitmapA.bitmap], bitmaps.bitmapB.offset - 2, bitmaps.bitmapA.color, bitmaps.orientation);
            // third call draw in the space cleared by the previous calls
            Matrices::drawBitmap(BITMAP_STORE[bitmaps.bitmapB.bitmap], bitmaps.bitmapB.offset, bitmaps.bitmapB.color, bitmaps.orientation);

            Display::drawAcceleration();

            // once all leds are set by clearing and drawing, flush the matrices
            // Matrices::write();

            vTaskDelay(90);

        } else {

            // TODO :: warn about unknown modus on display
            Serial.print("unknown modus: ");
            Serial.println(String(modus));
            vTaskDelay(100);

        }

        Display::writeClip(); // prepare the clip-buffer that will actually be drawn to the display

    }

}

void runLoopTaskGeneral(void* pvParameters) {

    while (true) {

#if USE_SERIAL_LOOP_OUTPUT == true
        Serial.print("loopMillis (avg): ");
        Serial.println(String(totalLoopMillis * 1.0 / totalLoopNumber, 2));
#endif

        if (Device::getCurrModus() == MODUS________ACCEL) {  // must refer to actual curr device modus or it will not happen due to prevMode

            Nowsrv::sendAcceleration();  // consumes ~1 millisecond

            if (USE__FORCE_MODUS_ACCEL || Device::getDeviceRole() == DEVICE_ROLE_____PRI) {

                if (bitmapPos > 64) {
                    bitmapPos = BITMAP_RESET_POS;
                }
                bitmapPos++;

                Device::currBitmaps.bitmapB.offset = bitmapPos;
                Device::currBitmaps.bitmapB.bitmap = (bitmap________e)((bitmapPos + 64) % 2);  // open and close mouth, keep the number going into modulo in positive range
                Device::currBitmaps.orientation = Device::getOrientation();
                Nowsrv::sendBitmaps(Device::currBitmaps);
            }

        } else {
            bitmapPos = BITMAP_RESET_POS;  // TODO :: this could be done when "modus", not "Device::getCurrModus()" != ACCEL, so it also resets when in fallback mode
        }

        vTaskDelay(100);

    }

}

void setup(void) {

    Serial.begin(9600);
    delay(2000);

    // https// forum.arduino.cc/t/shuffle-an-array-of-ints/333494/6
    // use a different seed value so that we don't get same result each time we run this program
    // this is an attempt to have different random word orders across device starts
    randomSeed(analogRead(A0));

    Serial.print("setup ");
    Serial.print(BLE_DEVICE_NAME);
    Serial.println(" ...");
    Serial.print("core: ");
    Serial.println(xPortGetCoreID());

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

    Orientation::powerup();
    delay(100);

    xTaskCreatePinnedToCore(runLoopTaskGeneral, "run-loop-general", 10000, NULL, 2, NULL, 0);
    xTaskCreatePinnedToCore(runLoopTaskDisplay, "run-loop-display", 10000, NULL, 2, NULL, 0);


    Serial.println("... setup");
}

/**
 * primary loop running on core-1
 * attempts have been made to have everything disturbing the microphone in terms of unwanted frequencies is happening here
 */
void loop() {

    millisAPri = millis();

    modus_________e modus = determineModus();
    if (modus == MODUS________FREQU) {
        Microphone::read();    // samples * 25 +
        Matrices::drawBars();  // 7ms
    }
    Matrices::write();  // only writes if required

#if USE_SERIAL_LOOP_OUTPUT == true
    uint64_t millisBPri = millis();
#endif

    // only writes if required, a single write consumes ~14ms
    // with clipBuffer a single write consumes ~5ms
    bool written = Display::writeCopy();

    // in frequency modus ~40ms to here

    durationAB = millis() - millisAPri;
    if (durationAB < 45) {
        delay(45 - durationAB);  // wait until 55 millis have passed since millisA
    }

    Orientation::read();

#if USE_SERIAL_LOOP_OUTPUT == true
    totalLoopMillis += (millis() - millisAPri);
    totalLoopNumber++;
#endif

    // ~48ms to here

    durationAC = millis() - millisAPri;
    if (durationAC < 50) {
        delay(50 - durationAC);  // wait until 60 millis have passed since millisA
    }

}
