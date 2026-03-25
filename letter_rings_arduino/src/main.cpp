#include <Arduino.h>

#include "Blesrv.h"
#include "Buttons.h"
#include "Device.h"
#include "Display.h"
#include "Matrices.h"
#include "Microphone.h"
#include "Nowsrv.h"
#include "Orientation.h"
#include "GamOL.h"
#include "Chars.h"

uint16_t pixelPos = 0;
int16_t labelPos = 33;
String mainLabel = "";
uint16_t labelWidth = 32;  // start value, will be recalculated upon first text change

String mainWord = "";
uint8_t wordProgress = 0;

uint64_t lastWordUpdateMillis = 0;
uint64_t lastLabelUpdateMillis = 0;
uint64_t lastGamOLThresholdMillis = 0;

const int16_t BITMAP_RESET_POS = -10;
int16_t bitmapPos = BITMAP_RESET_POS;

uint64_t millisAPri = 0;

#if USE_SERIAL_LOOP_OUTPUT == true
uint64_t totalLoopMillis = 0;
uint64_t totalLoopNumber = 0;
uint64_t lastLoopMillis = 0;
#endif

bool exceedsWordUpdateInterval(uint64_t currMillis) {
    return (currMillis - lastWordUpdateMillis) > WORD_UPDATE_INTERVAL_MS;
}

bool exceedsPartyLabelDuration(uint64_t currMillis) {
    return (currMillis - lastLabelUpdateMillis) > PARTY_LABEL_DURATION_MS;
}

/**
 * check if the GamOL animation has run a full cycle
 * will always return true until lastGamOLThresholdMillis has been set to a value higher than zero,
 * this is to prevent a GamOL animation from starting right after a device boot
 */
bool exceedsGamOLThresholdDuration(uint64_t currMillis) {
    if (lastGamOLThresholdMillis == 0) {
        return true;
    } else {
        return (currMillis - lastGamOLThresholdMillis) > GAMOL_______DURATION_MS;
    }

}

modus_________e determineModus() {

    uint64_t currMillis = millis();

    // label needs to be checked outside of MODUS________LABEL clause, so a correct decision can be made when in MODUS________PARTY
    if (Device::label != mainLabel) {
        labelWidth = Matrices::matrixA.getLabelWidth(Device::label);
        mainLabel = Device::label;
        lastLabelUpdateMillis = currMillis;
    }

    modus_________e modus = Device::getCurrModus();
    // if in accel mode and paired -> run pacman, fallback to previous mode if not paired
    if (modus == MODUS________ACCEL) {

        if (USE__FORCE_______GAMPM || Device::getDeviceRole() == DEVICE_ROLE_____PRI) {

            modus = MODUS____GAMPM_PRI;

        } else if (Device::getDeviceRole() == DEVICE_ROLE_____SEC) {

            modus = MODUS____GAMPM_SEC;

        } else {

            modus = Device::getPrevModus();

            if (Orientation::isAboveSignificantThreshold(ACCELERATION_SIG_THRES)) {
                if (exceedsGamOLThresholdDuration(currMillis)) { // init field upon first exeeding threshold
                    GamOL::readFieldState();
                }
                lastGamOLThresholdMillis = currMillis;
            }

            // keep the GamOL animation running even after a significant threshold event
            if (!exceedsGamOLThresholdDuration(currMillis)) { // Game of Life
                modus = MODUS________GAMOL;
            }

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

        modus_________e determModus = determineModus();

        orientation___e matricesOrientation = Device::getOrientation();
        if (matricesOrientation == ORIENTATION______UP && Orientation::getOrientation().y > 20) {
            Device::setOrientation(ORIENTATION____DOWN);
        }
        if (matricesOrientation == ORIENTATION____DOWN && Orientation::getOrientation().y < -20) {
            Device::setOrientation(ORIENTATION______UP);
        }

        // common display redraws (have to be in this loop to keep the clip logic in a single place)
        // this will happen at varying intervals, however all of them are in the same magnitude ~100-250ms
        Display::drawConfig(); // only draws when the needsConfigRedraw is true
        Display::drawConnection(); // only draws when the connection state changed
        Display::drawDeviceRole();  // little box indicators, only drawn after a change to the role
        Display::drawOrientation(); // will only draw if orientation was properly initialized
        Display::drawSignal(); // will always draw
        Display::drawMatrixState();  // I2C init states of matrices, only draws once

        if (determModus == MODUS________CHARS) {

            Chars::step();

            Matrices::clear(CLEAR_MATRIX_CANVAS | CLEAR_MATRIX___DISP);
            for (int i = 0; i < CHARS______________NUM; i++) {
                Matrices::drawChar(Chars::chars[i]);
            }

            vTaskDelay(100);

        } else if (determModus == MODUS________WORDS) {

            if (exceedsWordUpdateInterval(currMillis)) {
                uint64_t randomWordIndex = random(0, WORD_COUNT - 1);
                Device::word = WORDS[randomWordIndex];  // will be picked up some lines down
            }

            if (Device::word != mainWord) {
                mainWord = Device::word;
                wordProgress = 0;
                lastWordUpdateMillis = currMillis;
            }

            if (wordProgress < WORD_____TRANSITION_NUM) {
                wordProgress++;
                String word = WORDS[random(0, WORD_COUNT - 1)];
                Matrices::drawWord(word);
                Display::drawText(word);
                vTaskDelay(75);
            } else {
                if (wordProgress == WORD_____TRANSITION_NUM) {
                    wordProgress++;
                    Matrices::drawWord(mainWord);
                    Display::drawText(mainWord);
                }
                vTaskDelay(100);
            }

        } else if (determModus == MODUS________LABEL) {

            Matrices::drawLabel(mainLabel, labelPos);
            Display::drawText(mainLabel);

            if (labelPos < -labelWidth) {  // all the way out to the left
                labelPos = 32;             // reset to right border
            }
            labelPos--;

            vTaskDelay(max(50.0f, 150.0f - Orientation::getOrientation().z));

        } else if (determModus == MODUS________FREQU) {  // frequency

            // Matrices are drawn in primary loop
            Display::drawFrequ();

            vTaskDelay(100);  // effectively do nothing, just wait a bit

        } else if (determModus == MODUS________BREAK) {  // frequency

            Matrices::clear(CLEAR_MATRIX_CANVAS | CLEAR_MATRIX___DISP);
            Matrices::drawPixel(pixelPos, 6, LED_ON);

            Display::drawText("");

            if (pixelPos > 31) {  // all the way out to the left
                pixelPos = 0;     // reset to right border
            }
            pixelPos++;

            vTaskDelay(250);

        } else if (determModus == MODUS____GAMPM_PRI || determModus == MODUS____GAMPM_SEC) {

            bitmaps_______t bitmaps = Device::currBitmaps;

            Matrices::clear(CLEAR_MATRIX_CANVAS);
            // first two calls clear the bitmap ahead and behind
            Matrices::drawBitmapWithOrientation(BITMAP_STORE[bitmaps.bitmapA.bitmap], bitmaps.bitmapB.offset + 2, LED_OFF, bitmaps.orientation);
            Matrices::drawBitmapWithOrientation(BITMAP_STORE[bitmaps.bitmapA.bitmap], bitmaps.bitmapB.offset - 2, LED_OFF, bitmaps.orientation);
            // third call draws into the space cleared by the previous calls
            Matrices::drawBitmapWithOrientation(BITMAP_STORE[bitmaps.bitmapB.bitmap], bitmaps.bitmapB.offset, LED_ON, bitmaps.orientation);

            Display::drawAcceleration();

            vTaskDelay(100);

        } else if (determModus == MODUS________GAMOL) {

            GamOL::drawFieldState();
            GamOL::stepFieldState();

            vTaskDelay(75);

        } else {

            Serial.print("unknown determModus: ");
            Serial.println(String(determModus));
            vTaskDelay(100);

        }

        // prepare the clip-buffer that will actually be drawn to the display
        Display::writeClip();

    }

}

// void logValues(float values[ACCELERATION___SAMPLES]) {
//     Serial.print("{");
//     for (uint8_t i = 0; i < ACCELERATION___SAMPLES; i++) {
//         Serial.print(String(values[i], 3));
//         if (i < ACCELERATION___SAMPLES - 1) {
//             Serial.print(",");
//         }
//     }
//     Serial.print("}");
// }

#if DEVICE____________LEFT == true
void applyGestureModus(modus_________e gestureModus) {
    if (Device::getCurrModus() == MODUS________ACCEL && Device::getPrevModus() != gestureModus) { // if in ACCEL, set to WORDS, then reapply ACCEL
        Device::setCurrModus(gestureModus);
        Device::setCurrModus(MODUS________ACCEL);
        Blesrv::writeModus();
    } else if (Device::getCurrModus() != gestureModus) {
        Device::setCurrModus(gestureModus);
        Blesrv::writeModus();
    }
}
#endif

void runLoopTaskGeneral(void* pvParameters) {

    uint64_t counterGeneral = 0;
    while (true) {

#if USE_SERIAL_LOOP_OUTPUT == true
        Serial.print("loopMillis (avg): ");
        Serial.print(String(totalLoopMillis * 1.0 / totalLoopNumber, 2));
        Serial.print(", totalLoopNumber: ");
        Serial.print(String(totalLoopNumber));
        Serial.print(", lastMillis: ");
        Serial.println(String(lastLoopMillis));
#endif

        if (counterGeneral % 2 == 0) { // every second iteration of this loop
            if (Device::getCurrModus() == MODUS________ACCEL) {  // must refer to actual curr device modus or it will not happen due to prevMode

                modus_________e determModus = determineModus(); // see what central logic tells about modus
                if (determModus == MODUS____GAMPM_PRI) {

                    if (bitmapPos > 64) {
                        bitmapPos = BITMAP_RESET_POS;
                    }
                    bitmapPos++;

                    Device::currBitmaps.bitmapB.offset = bitmapPos;
                    Device::currBitmaps.bitmapB.bitmap = (bitmap________e)((bitmapPos + 64) % 2);  // open and close mouth, keep the number going into modulo in positive range
                    Device::currBitmaps.orientation = Device::getOrientation();

                }

            } else {
                bitmapPos = BITMAP_RESET_POS;  // TODO :: this could be done when "modus", not "Device::getCurrModus()" != ACCEL, so it also resets when in fallback mode
            }
        }

        // if (Orientation::isAboveSignificantThreshold((float)ACCELERATION_TAP_THRES, ACCELERATION___SAMPLES - 5, ACCELERATION___SAMPLES - 3)) {
        //     acceleration_t accelerationsA = Orientation::getAccelerationsA();
        //     Serial.println("{");
        //     logValues(accelerationsA.valuesX);
        //     Serial.println(",");
        //     logValues(accelerationsA.valuesY);
        //     Serial.println(",");
        //     logValues(accelerationsA.valuesZ);
        //     Serial.println();
        //     Serial.println("}");
        // }

#if DEVICE____________LEFT == true

        double coefG = Orientation::getCoefGThreshold();
        bool isGestureW = Orientation::matchGesture(ACCEL_W, 1.87 * coefG); // 1.12@0.6 - 1.40@0.75 - 1.68@0.9
        bool isGestureF = Orientation::matchGesture(ACCEL_F, 2.07 * coefG); // 1.42@0.6 - 1.55@0.75 - 1.86@0.9
        bool isGestureC = Orientation::matchGesture(ACCEL_C, 2.13 * coefG); // 1.28@0.6 - 1.60@0.75 - 1.92@0.9

        if (isGestureW) {
            applyGestureModus(MODUS________WORDS);
            // TODO :: reset word if the device is not connected to immediately see that words is active
            // TODO :: add functionality in app that updates words immediately when entering words mode
        } else if (isGestureF) {
            applyGestureModus(MODUS________PARTY);
        } else if (isGestureC) {
            applyGestureModus(MODUS________CHARS);
        }

#endif

        counterGeneral++;
        vTaskDelay(50);

    }

}

/**
 * primary loop running on core-1
 * attempts have been made that everything disturbing the microphone in terms of unwanted frequencies is happening here
 * however, ESP-NOW and Bluetooth still consume energy and produce unwanted signal since the callback functions run async from this loop
 */
void runLoopTaskPrimary(void* pvParameters) {

    while (true) {

        millisAPri = millis();

        modus_________e determModus = determineModus();
        if (determModus == MODUS________FREQU) {
            Microphone::read();
            Matrices::drawBars();  // 7ms
        }

        // ~29ms here when in frequency mode

        while ((millis() - millisAPri) < 30) {
            vTaskDelay(1);
            yield();
        }

        modus_________e deviceModus = Device::getCurrModus();
        if (deviceModus == MODUS________ACCEL) { // must refer to actual curr device modus or it will not happen due to prevMode
            Nowsrv::sendDeviceData();
        }

        Matrices::write();  // only writes if required

        // only writes if required
        // with clipBuffer a single write consumes ~5ms
        Display::writeCopy();

        // ~42ms here when in frequ/accel mode

        while ((millis() - millisAPri) < 43) {
            vTaskDelay(1);
            yield();
        }

        Orientation::read();

#if USE_SERIAL_LOOP_OUTPUT == true
        lastLoopMillis = (millis() - millisAPri);
        totalLoopMillis += lastLoopMillis;
        totalLoopNumber++;
#endif

        // ~49ms here when in frequ/accel mode

        while ((millis() - millisAPri) < MAIN_LOOP_______DEST_MS) { // wait until 200 millis have passed since millisA
            yield();
        }

        if (deviceModus == MODUS________ACCEL) { // must refer to actual curr device modus or it will not happen due to prevMode
            uint64_t millisWaitB = Orientation::getMagnitudesB().millisWait;
            uint64_t millisWaitA = Orientation::getMagnitudesA().millisWait;
            // some extra delay to sync devices, once synced this would be in the one-digit millisecond range
            // some safety added to not get caught with long delays in edge cases
            if (millisWaitA < millisWaitB && millisWaitA >= 4 && millisWaitA < (MAIN_LOOP_______DEST_MS * 2)) {
                uint64_t extraDelay = millisWaitA / 4L;
#if USE_SERIAL_SYNC_OUTPUT == true
                Serial.print(DEVICE____________SIDE);
                Serial.print(", ms: ");
                Serial.print(String(millis()));
                Serial.print(", extraDelay: ");
                Serial.println(String(extraDelay));
#endif
                vTaskDelay(extraDelay);
            }

        }

    }

}

#if USE_SERIAL__MIC_OUTPUT == true
void runLoopTaskMicVals(void* pvParameters) {

    while (true) {

        Serial.print("basis: ");
        Serial.println(String(Microphone::basis, 2));
        Serial.print("scale: ");
        Serial.println(String(Microphone::scale, 5));

        vTaskDelay(2000);
    }

}
#endif

void setup(void) {

    Serial.begin(9600);
    delay(2000);

    // https// forum.arduino.cc/t/shuffle-an-array-of-ints/333494/6
    // use a different seed value so that we don't get same result each time we run this program
    // this is an attempt to have different random word orders across device starts and therefore different word orders
    randomSeed(analogRead(A0));

    String separator = "----------------------";
    Serial.println(separator);
    Serial.println(DEVICE____________NAME);
    Serial.println(separator);

    Display::powerup();
    delay(100);

    Matrices::powerup();
    delay(100);

    Microphone::powerup();
    delay(100);

    Buttons::powerup();
    delay(100);

    Chars::powerup();
    delay(100);

    Orientation::powerup();
    delay(100);

    Serial.println(separator);

    Nowsrv::powerup();
    delay(100);

    Blesrv::powerup();
    delay(100);

    xTaskCreatePinnedToCore(runLoopTaskGeneral, "run-loop-general", 10000, NULL, 2, NULL, 0);
    xTaskCreatePinnedToCore(runLoopTaskDisplay, "run-loop-display", 10000, NULL, 2, NULL, 0);
    xTaskCreatePinnedToCore(runLoopTaskPrimary, "run-loop-primary", 10000, NULL, 2, NULL, 1); // run on primary core
#if USE_SERIAL__MIC_OUTPUT == true
    xTaskCreatePinnedToCore(runLoopTaskMicVals, "run-loop-micvals", 10000, NULL, 2, NULL, 0);
#endif

    Serial.println(separator);
}


void loop() {

    delay(1000);

}
