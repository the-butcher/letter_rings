#include <Arduino.h>

#include "Blesrv.h"
#include "Buttons.h"
#include "Device.h"
#include "Display.h"
#include "Matrices.h"
#include "Microphone.h"
#include "Nowsrv.h"
#include "Orientation.h"

int16_t labelPos = 33;
String mainLabel = "";
uint16_t labelWidth = 32;  // start value, will be recalculated upon first text change

String mainWord = "";
uint8_t wordProgress = 0;
const uint8_t WORD_TRANSITION_COUNT = 10;

uint64_t lastWordUpdateMillis = 0;
uint64_t lastLabelUpdateMillis = 0;
uint64_t lastOrientationUpdateMillis = 0;

const int16_t BITMAP_RESET_POS = -10;
int16_t bitmapPos = BITMAP_RESET_POS;

uint64_t millisAPri = 0;
uint64_t millisBPri = 0;
uint64_t millisCPri = 0;
uint64_t durationAB = 0;
uint64_t durationAC = 0;

bool exceedsWordUpdateInterval(uint64_t currMillis) {
    return (currMillis - lastWordUpdateMillis) > WORD_UPDATE_INTERVAL_MS;
}

bool exceedsPartyLabelDuration(uint64_t currMillis) {
    return (currMillis - lastLabelUpdateMillis) > PARTY_LABEL_DURATION_MS;
}

modus_________e determineModus() {

    modus_________e modus = Device::getCurrModus();
    // if in accel mode and paired -> run pacman, fallback to previous mode otherwise
    if (modus == MODUS________ACCEL) {
        if (Device::getDeviceRole() == DEVICE_ROLE_____ANY && !FORCE_ACCEL) {  // (not primary, not secondary) = below coefficient threshold
            modus = Device::getPrevModus();
        }
    }

    // if in party mode, make a decision for either label or frequency
    if (modus == MODUS________PARTY) {
        modus = exceedsPartyLabelDuration(millis()) ? MODUS________FREQU : MODUS________LABEL;
    }

    return modus;
}

void runSecondaryLoopTask(void* pvParameters) {

    Serial.println("runSecondaryLoopTask");

    while (true) {

        uint64_t currMillis = millis();

        // label needs to be checked outside of MODUS________LABEL clause, so a correct decision can be made when in MODUS________PARTY
        if (Device::label != mainLabel) {
            labelWidth = Matrices::matrixA.getLabelWidth(Device::label);
            mainLabel = Device::label;
            lastLabelUpdateMillis = currMillis;
        }

        modus_________e modus = determineModus();

        if (Display::needsStatusRedraw) {
            Display::drawStatus(modus);
        }

        // TODO :: this probably has to go to the regular loop (but wont happen regularly, so likely not important)
        if (Matrices::needsBrightnessUpdate) {
            Matrices::updateBrightness();
        }

        // update position (max update count/second = 10)
        if ((currMillis - lastOrientationUpdateMillis) > 100) {

            // TODO :: reintroduce euler orientation every n-th read
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

            if (Device::getCurrModus() == MODUS________ACCEL) {  // must refer to actual curr device modus or it will not happen due to prevMode

                Display::drawAcceleration();
                Nowsrv::sendAcceleration();  // consumes ~1 millisecond

                if (FORCE_ACCEL || Device::getDeviceRole() == DEVICE_ROLE_____PRI) {

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

            if (modus == MODUS________FREQU) {
                Display::drawFrequ();
            }

            Display::drawDeviceRole();  // little box indicators (TODO :: different colors for better readability)

            lastOrientationUpdateMillis = currMillis;
        }

        if (modus == MODUS________WORDS) {

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
                vTaskDelay(50);
            } else {
                if (wordProgress == WORD_TRANSITION_COUNT) {
                    wordProgress++;
                    Display::drawText(mainWord);
                    Matrices::drawWord(mainWord);  // TODO :: check if it is smoother to first draw all, then flush all
                }
                vTaskDelay(100);
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
            vTaskDelay(max(50.0, 150 - Orientation::getOrientation().z));

        } else if (modus == MODUS________FREQU) {  // frequency

            // Microphone::read();

            // this may have to happen in the primary loop to align Microphone::read() and Matrices::drawBars()
            // Matrices::drawBars();  // 7ms
            // Matrices::write();

            vTaskDelay(100);  // effectively do nothing, just wait a bit

        } else if (modus == MODUS________ACCEL) {

            bitmaps_______t bitmaps = Device::currBitmaps;

            // first two calls clear the bitmap ahead and behind
            Matrices::drawBitmap(BITMAP_STORE[bitmaps.bitmapA.bitmap], bitmaps.bitmapB.offset + 2, bitmaps.bitmapA.color, bitmaps.orientation);
            Matrices::drawBitmap(BITMAP_STORE[bitmaps.bitmapA.bitmap], bitmaps.bitmapB.offset - 2, bitmaps.bitmapA.color, bitmaps.orientation);
            // third call draw in the space cleared by the previous calls
            Matrices::drawBitmap(BITMAP_STORE[bitmaps.bitmapB.bitmap], bitmaps.bitmapB.offset, bitmaps.bitmapB.color, bitmaps.orientation);

            // once all leds are set by clearing and drawing, flush the matrices
            // Matrices::write();

            vTaskDelay(90);

        } else {

            // TODO :: warn about unknown modus on display
            Serial.print("unknown modus: ");
            Serial.println(String(modus));
            vTaskDelay(100);
        }

        // vTaskDelay(250);
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

    xTaskCreatePinnedToCore(runSecondaryLoopTask, "run-secondary-loop", 10000, NULL, 2, NULL, 0);

    Serial.println("... setup");
}

void loop() {

    millisAPri = millis();

    modus_________e modus = determineModus();
    if (modus == MODUS________FREQU) {
        Microphone::read();
        Matrices::drawBars();  // 7ms
    }
    if (Matrices::needsWrite) {
        Matrices::write();
    }
    if (Display::needsWrite) {
        Display::write();
    }

    millisBPri = millis();
    durationAB = millisBPri - millisAPri;
    if (durationAB < 40) {
        delay(40 - durationAB);  // wait until 40 millis have passed since millisA
    }

    Orientation::read();

    millisCPri = millis();
    durationAC = millisCPri - millisAPri;
    if (durationAC < 50) {
        delay(50 - durationAC);  // wait until 50 millis have passed since millisA
    }
}
