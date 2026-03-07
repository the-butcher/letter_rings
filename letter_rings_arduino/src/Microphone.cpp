#include "Microphone.h"

unsigned int Microphone::sampling_period_us = round(1000000 * (1.0 / AUDIO____SAMPLING_FREQ));
double Microphone::vReal[AUDIO__________SAMPLES];
double Microphone::vImag[AUDIO__________SAMPLES];
ArduinoFFT<double> Microphone::FFT(Microphone::vReal, Microphone::vImag, AUDIO__________SAMPLES, AUDIO____SAMPLING_FREQ);
unsigned long Microphone::newTime;
int Microphone::bandValues[AUDIO________NUM_BANDS];

int Microphone::buckValueMax;
int Microphone::buckValues[AUDIO________NUM_BANDS];
int Microphone::lineValues[AUDIO________NUM_BANDS];
int Microphone::peakValues[AUDIO________NUM_BANDS];
double Microphone::dlt1Values[AUDIO________NUM_BANDS];
double Microphone::dlt2Values[AUDIO________NUM_BANDS];
uint8_t Microphone::decay = 50;
uint64_t Microphone::signal = 0;
double Microphone::scale = 0.005;
double Microphone::basis = 2000;

double Microphone::fitFAverag;
double Microphone::fitXValues[AUDIO________NUM_BANDS];      // a list of ascending double values
double Microphone::fitYValues[AUDIO________NUM_BANDS];      // bandValues as double values
double Microphone::fitFValues[AUDIO________NUM_BANDS];      // values along the fit curve
int Microphone::bandScaled[AUDIO________NUM_BANDS];         // band values scaled from fit curve to fit average
double Microphone::coefValues[AUDIO________NUM_ORDER + 1];  // the curve coefficients after fitting
double Microphone::curvValues[AUDIO________NUM_BANDS];

bool Microphone::powerup() {

    Serial.print("sampling period us: ");
    Serial.println(Microphone::sampling_period_us);

    double c = 7.5;
    double b = 10.0;

    // roughly 60Hz to 10000Hz
    Microphone::buckValueMax = AUDIO__________SAMPLES / 4;

    for (int i = 0; i < AUDIO________NUM_BANDS; i++) {

        // https://www.desmos.com/calculator/qrusqcv1jx?lang=de
        Microphone::buckValues[i] = 0 + round(pow(i * 1.0 / AUDIO________NUM_BANDS, 2.00) * Microphone::buckValueMax);

        // https://www.desmos.com/calculator/o8ajvpoceu?lang=de
        Microphone::curvValues[i] = 1;  // 1 - pow((i - c) / b, 2);

        Serial.print(i);
        Serial.print(" -> ");
        Serial.println(String(Microphone::buckValues[i]));
        Microphone::fitXValues[i] = i;  // initialize fit-x values
    }

https:  // github.com/stg/ESP32-S3-FastAnalogRead/blob/master/ESP32-S3-FastAnalogRead.ino
    fadcInit(1, AUDIO______________PIN);

    return true;
}

void Microphone::read() {

    // Reset bandValues[]
    for (int i = 0; i < AUDIO________NUM_BANDS; i++) {
        Microphone::bandValues[i] = 0;
    }

    // collect samples
    double signalAvg = 0;
    uint16_t analogValue;
    for (int i = 0; i < AUDIO__________SAMPLES; i++) {
        Microphone::newTime = micros();
        analogValue = analogReadFast(7);  // analogRead(AUDIO______________PIN);  // A conversion takes about 9.7uS on an ESP32
        signalAvg += analogValue;
        Microphone::vReal[i] = analogValue;
        Microphone::vImag[i] = 0;
        while ((micros() - Microphone::newTime) < Microphone::sampling_period_us) {
            // do nothing, just let time pass
        }
    }
    signalAvg = signalAvg / AUDIO__________SAMPLES;
    Microphone::signal = (uint64_t)round(signalAvg);

    // https://leobot.net/tutorial/1067?srsltid=AfmBOopGwK3bNLB8GH9lWKwbYeUN1UAsiPBQsh7GJRLy7i_L0Uc4ghWd
    for (int i = 0; i < AUDIO__________SAMPLES; i++) {
        Microphone::vReal[i] = Microphone::vReal[i] - signalAvg;
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

    double f = 0.001;  // the speed at which the low pass filter adapts
    Microphone::fitFAverag = 0;
    double x;
    double y;
    for (int i = 0; i < AUDIO________NUM_BANDS; i++) {
        x = i * 1.0;
        y = Microphone::coefValues[0] * pow(x, 3) + Microphone::coefValues[1] * pow(x, 2) + Microphone::coefValues[2] * x + Microphone::coefValues[3];
        Microphone::fitFValues[i] = Microphone::fitFValues[i] * (1 - f) + y * f;  // low pass
        Microphone::fitFAverag += Microphone::fitFValues[i];
    }
    Microphone::fitFAverag /= AUDIO________NUM_BANDS;

    // 28ms to here

    for (int i = 0; i < AUDIO________NUM_BANDS; i++) {
        Microphone::bandScaled[i] = Microphone::bandValues[i] * Microphone::curvValues[i] * Microphone::fitFAverag / Microphone::fitFValues[i];
    }

    for (int i = 0; i < AUDIO________NUM_BANDS; i++) {

        double delta = Microphone::bandScaled[i] - Microphone::basis;

        Microphone::dlt1Values[i] = Microphone::dlt1Values[i] * (1 - Microphone::decay / 100.0);  // decay curr mark
        Microphone::dlt2Values[i] = Microphone::dlt2Values[i] * 0.95;                             // decay peak mark

        Microphone::dlt1Values[i] = max(Microphone::dlt1Values[i], delta);
        Microphone::dlt2Values[i] = max(Microphone::dlt2Values[i], delta);

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
        Microphone::scale *= 0.999;
        // Serial.print("decrease scale: ");
        // Serial.println(String(Microphone::scale, 5));
    } else {
        if (Microphone::scale < 0.01) {
            Microphone::scale *= 1.001;
            // Serial.print("increase scale: ");
            // Serial.println(String(Microphone::scale, 5));
        } else {
            // Serial.println("increase scale: skip");
        }
    }

    if (numOutliers0015L > 2) {
        Microphone::basis *= 0.999;
        // Serial.print("decrease scale: ");
        // Serial.println(String(Microphone::scale, 5));
    } else {
        Microphone::basis *= 1.001;
    }

    // 28ms to here
}

bool Microphone::depower() {
    // nothing
    return true;
}