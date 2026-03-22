#include "Microphone.h"

unsigned int Microphone::sampling_period_us = round(1000000 * (1.0 / AUDIO____SAMPLING_FREQ));
double Microphone::vReal[AUDIO__________SAMPLES];
double Microphone::vImag[AUDIO__________SAMPLES];
ArduinoFFT<double> Microphone::FFT(Microphone::vReal, Microphone::vImag, AUDIO__________SAMPLES, AUDIO____SAMPLING_FREQ);
unsigned long Microphone::lastSampleTime;
int Microphone::bandValues[AUDIO________NUM_BANDS];

int Microphone::buckValueMax;
int Microphone::buckValues[AUDIO________NUM_BANDS];
int Microphone::lineValues[AUDIO________NUM_BANDS];
int Microphone::peakValues[AUDIO________NUM_BANDS];
double Microphone::dlt1Values[AUDIO________NUM_BANDS];
double Microphone::dlt2Values[AUDIO________NUM_BANDS];
uint8_t Microphone::decay = 20;

uint64_t Microphone::signalSum = 0;
uint64_t Microphone::signalNum = 0;
uint64_t Microphone::signalAvg = 0;

/**
 * used to fit the signal to the 8 pixels available
 * no     music: ~0.002
 * silent music: ~0.002
 * load (private): 0.0008-0.0006
 */
double Microphone::scale = AUDIO________MAX_SCALE;
/**
 * used for a bottomline
 * no     music: ~ 800
 * silent music: ~1200
 * load (private): ~1800-2500
 */
double Microphone::basis = 2000;

double Microphone::fitFAverag;
double Microphone::fitXValues[AUDIO________NUM_BANDS];      // a list of ascending double values
double Microphone::fitYValues[AUDIO________NUM_BANDS];      // bandValues as double values
double Microphone::fitFValues[AUDIO________NUM_BANDS];      // values along the fit curve
int Microphone::bandScaled[AUDIO________NUM_BANDS];         // band values scaled from fit curve to fit average
double Microphone::coefValues[AUDIO________NUM_ORDER + 1];  // the curve coef after fitting
double Microphone::curvValues[AUDIO________NUM_BANDS];

bool Microphone::powerup() {

    double c = 7.0;
    double b = 9.0;

    // roughly 60Hz to 10000Hz
    Microphone::buckValueMax = AUDIO__________SAMPLES / 4;

    for (int i = 0; i < AUDIO________NUM_BANDS; i++) {

        // https://www.desmos.com/calculator/qrusqcv1jx?lang=de
        Microphone::buckValues[i] = 0 + round(pow(i * 1.0 / AUDIO________NUM_BANDS, 2.00) * Microphone::buckValueMax);

        // https://www.desmos.com/calculator/o8ajvpoceu?lang=de
        Microphone::curvValues[i] = 1; // 1 - pow((i - c) / b, 2); // 1

        Serial.printf("frequ bucket %02d -> %03d\n", i, Microphone::buckValues[i]);
        Microphone::fitXValues[i] = i;  // initialize fit-x values
    }

    // https://github.com/stg/ESP32-S3-FastAnalogRead/blob/master/ESP32-S3-FastAnalogRead.ino
    fadcInit(1, AUDIO______________PIN);

    return true;
}

uint64_t Microphone::getSignalAvg() {
    return  Microphone::signalAvg;
}

void Microphone::read() {

    // Reset bandValues[]
    for (int i = 0; i < AUDIO________NUM_BANDS; i++) {
        Microphone::bandValues[i] = 0;
    }

    // collect samples
    uint16_t analogValue;
    double sampleSum = 0;
    for (int i = 0; i < AUDIO__________SAMPLES; i++) {
        Microphone::lastSampleTime = micros();
        analogValue = analogReadFast(7); // A5 is on channel 7  // analogRead(AUDIO______________PIN);  // A conversion takes about 9.7uS on an ESP32
        sampleSum += analogValue;
        Microphone::vReal[i] = analogValue;
        Microphone::vImag[i] = 0;
        while ((micros() - Microphone::lastSampleTime) < Microphone::sampling_period_us) {
            // do nothing, just let time pass
        }
    }
    Microphone::signalSum += (uint64_t)round(sampleSum / AUDIO__________SAMPLES);
    Microphone::signalNum++;
    Microphone::signalAvg = Microphone::signalSum / Microphone::signalNum;

    // https://leobot.net/tutorial/1067?srsltid=AfmBOopGwK3bNLB8GH9lWKwbYeUN1UAsiPBQsh7GJRLy7i_L0Uc4ghWd
    for (int i = 0; i < AUDIO__________SAMPLES; i++) {
        Microphone::vReal[i] = Microphone::vReal[i] - Microphone::signalAvg;
    }

    // 13ms to here

    // compute FFT
    Microphone::FFT.dcRemoval();
    Microphone::FFT.windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    Microphone::FFT.compute(FFT_FORWARD);
    Microphone::FFT.complexToMagnitude();

    // 26ms to here

    // associate samples with bands
    int minSampleIndex;
    int maxSampleIndex;
    for (int bandIndex = 0; bandIndex < AUDIO________NUM_BANDS; bandIndex++) {
        minSampleIndex = Microphone::buckValues[bandIndex];
        maxSampleIndex = bandIndex < AUDIO________NUM_BANDS - 1 ? Microphone::buckValues[bandIndex + 1] : Microphone::buckValueMax;
        for (int sampleIndex = minSampleIndex; sampleIndex < maxSampleIndex; sampleIndex++) {
            Microphone::bandValues[bandIndex] += (int)(Microphone::vReal[sampleIndex]);
        }
    }

    // 27ms to here

    // get a shape along frequency bands
    // this is done to get a curve fit that still allows for local variation
    for (int i = 0; i < AUDIO________NUM_BANDS; i++) {
        Microphone::fitYValues[i] = Microphone::bandValues[i] * 1.0;
    }
    fitCurve(AUDIO________NUM_ORDER, AUDIO________NUM_BANDS, Microphone::fitXValues, Microphone::fitYValues, AUDIO________NUM_ORDER + 1, Microphone::coefValues);

    // 27ms to here

    Microphone::fitFAverag = 0;
    double x;
    double y;
    for (int i = 0; i < AUDIO________NUM_BANDS; i++) {
        x = i * 1.0;
        y = Microphone::coefValues[0] * pow(x, 3) + Microphone::coefValues[1] * pow(x, 2) + Microphone::coefValues[2] * x + Microphone::coefValues[3];
        Microphone::fitFValues[i] = Microphone::fitFValues[i] * (1 - AUDIO_FIT________DECAY) + y * AUDIO_FIT________DECAY;  // low pass
        Microphone::fitFAverag += Microphone::fitFValues[i];
    }
    Microphone::fitFAverag /= AUDIO________NUM_BANDS;

    // 28ms to here

    for (int i = 0; i < AUDIO________NUM_BANDS; i++) {
        Microphone::bandScaled[i] = Microphone::bandValues[i] * Microphone::curvValues[i] * Microphone::fitFAverag / Microphone::fitFValues[i];
    }

    double delta;
    for (int i = 0; i < AUDIO________NUM_BANDS; i++) {

        delta = Microphone::bandScaled[i] - Microphone::basis;

        Microphone::dlt1Values[i] = Microphone::dlt1Values[i] * (1 - Microphone::decay / 100.0);  // decay curr mark
        Microphone::dlt2Values[i] = Microphone::dlt2Values[i] * AUDIO_PEAK_______DECAY;                             // decay peak mark, 0.96 means 0.04 peak decay

        Microphone::dlt1Values[i] = max(Microphone::dlt1Values[i], delta); // push curr mark
        Microphone::dlt2Values[i] = max(Microphone::dlt2Values[i], delta); // push peak mark

        Microphone::lineValues[i] = round(Microphone::dlt1Values[i] * Microphone::scale);
        Microphone::peakValues[i] = round(Microphone::dlt2Values[i] * Microphone::scale);
    }

    // 28ms to here

    uint8_t numOutliers0015U = 0;
    uint8_t numOutliers0015L = 0;
    for (int i = 0; i < AUDIO________NUM_BANDS; i++) {
        if (Microphone::lineValues[i] > 4) {
            numOutliers0015U++;
        } else if (Microphone::bandValues[i] < Microphone::basis) {
            numOutliers0015L++;
        }
    }

    if (numOutliers0015U > 0) {
        Microphone::scale *= AUDIO_BASE_SCALE_DECAY; // decreases the value
        // Serial.print("decrease scale: ");
        // Serial.println(String(Microphone::scale, 5));
    } else {
        if (Microphone::scale < AUDIO________MAX_SCALE) { // limit scale increase or noise will dominate when rather silent
            Microphone::scale /= AUDIO_BASE_SCALE_DECAY; // increases the value
            // Serial.print("increase scale: ");
            // Serial.println(String(Microphone::scale, 5));
        } else {
            // Serial.println("increase scale: skip");
        }
    }

    if (numOutliers0015L > 2) {
        Microphone::basis *= AUDIO_BASE_SCALE_DECAY; // decreases the value
    } else {
        Microphone::basis /= AUDIO_BASE_SCALE_DECAY; // increases the value
    }

    // 28ms to here
}

bool Microphone::depower() {
    // nothing
    return true;
}