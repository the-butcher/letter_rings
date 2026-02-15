#include "Microphone.h"

unsigned int Microphone::sampling_period_us = round(1000000 * (1.0 / AUDIO____SAMPLING_FREQ));
double Microphone::vReal[AUDIO__________SAMPLES];
double Microphone::vImag[AUDIO__________SAMPLES];
ArduinoFFT<double> Microphone::FFT(Microphone::vReal, Microphone::vImag, AUDIO__________SAMPLES, AUDIO____SAMPLING_FREQ);
unsigned long Microphone::newTime;
int Microphone::bandValues[AUDIO________NUM_BANDS];
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

    // https://www.desmos.com/calculator/o8ajvpoceu?lang=de
    double c = 5;
    double b = 12;
    for (int i = 0; i < AUDIO________NUM_BANDS; i++) {
        Microphone::buckValues[i] = 2 + round(pow(i * 1.0 / AUDIO________NUM_BANDS, 1.75) * (AUDIO__________SAMPLES / 2));  // no buckets on the lower end if the exponent gets too high
        Microphone::curvValues[i] = 1 - pow((i - c) / b, 2);
        Serial.print(i);
        Serial.print(" -> ");
        Serial.println(String(Microphone::curvValues[i], 2));
        Microphone::fitXValues[i] = i;  // initialize fit-x values
    }

    return true;
}

// https://forum.arduino.cc/t/sorting-an-array/49201/9
void isort(double* a, int n) {
    for (int i = 1; i < n; ++i) {
        int j = a[i];
        int k;
        for (k = i - 1; (k >= 0) && (j < a[k]); k--) {
            a[k + 1] = a[k];
        }
        a[k + 1] = j;
    }
}

void Microphone::sample() {

    // Reset bandValues[]
    for (int i = 0; i < AUDIO________NUM_BANDS; i++) {
        Microphone::bandValues[i] = 0;
    }

    // collect samples
    Microphone::signal = 0;
    uint16_t analogValue;
    for (int i = 0; i < AUDIO__________SAMPLES; i++) {
        Microphone::newTime = micros();
        analogValue = analogRead(AUDIO______________PIN);  // A conversion takes about 9.7uS on an ESP32
        Microphone::signal += analogValue;
        Microphone::vReal[i] = analogValue;
        Microphone::vImag[i] = 0;
        while ((micros() - Microphone::newTime) < Microphone::sampling_period_us) { /* chill */
        }
    }
    Microphone::signal /= AUDIO__________SAMPLES;

    // compute FFT
    Microphone::FFT.dcRemoval();
    Microphone::FFT.windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    Microphone::FFT.compute(FFT_FORWARD);
    Microphone::FFT.complexToMagnitude();

    // associate samples with bands
    int minSampleIndex;
    int maxSampleIndex;
    for (int bandIndex = 0; bandIndex < AUDIO________NUM_BANDS; bandIndex++) {
        minSampleIndex = Microphone::buckValues[bandIndex];
        maxSampleIndex = bandIndex < AUDIO________NUM_BANDS - 1 ? Microphone::buckValues[bandIndex + 1] : AUDIO__________SAMPLES / 2;
        for (int sampleIndex = minSampleIndex; sampleIndex < maxSampleIndex; sampleIndex++) {
            Microphone::bandValues[bandIndex] += (int)(Microphone::vReal[sampleIndex]);
        }
    }

    for (int i = 0; i < AUDIO________NUM_BANDS; i++) {
        Microphone::fitYValues[i] = Microphone::bandValues[i] * 1.0;
    }
    fitCurve(AUDIO________NUM_ORDER, AUDIO________NUM_BANDS, Microphone::fitXValues, Microphone::fitYValues, AUDIO________NUM_ORDER + 1, Microphone::coefValues);

    double f = 0.001;  // the speed at which the low pass filter adapts
    Microphone::fitFAverag = 0;
    for (int i = 0; i < AUDIO________NUM_BANDS; i++) {
        double x = i;
        double y = Microphone::coefValues[0] * pow(x, 3) + Microphone::coefValues[1] * pow(x, 2) + Microphone::coefValues[2] * x + Microphone::coefValues[3];
        Microphone::fitFValues[i] = Microphone::fitFValues[i] * (1 - f) + y * f;  // low pass
        Microphone::fitFAverag += Microphone::fitFValues[i];
    }
    Microphone::fitFAverag /= AUDIO________NUM_BANDS;

    for (int i = 0; i < AUDIO________NUM_BANDS; i++) {
        Microphone::bandScaled[i] = Microphone::bandValues[i] * Microphone::curvValues[i] * Microphone::fitFAverag / Microphone::fitFValues[i];
    }

    for (int i = 0; i < AUDIO________NUM_BANDS; i++) {

        double delta = Microphone::bandScaled[i] - Microphone::basis;

        Microphone::dlt1Values[i] = Microphone::dlt1Values[i] * (1 - Microphone::decay / 100.0);  // decay curr mark
        Microphone::dlt2Values[i] = Microphone::dlt2Values[i] * 0.92;                             // decay peak mark

        Microphone::dlt1Values[i] = max(Microphone::dlt1Values[i], delta);
        Microphone::dlt2Values[i] = max(Microphone::dlt2Values[i], delta);

        Microphone::lineValues[i] = round(0.0 + round(Microphone::dlt1Values[i] * Microphone::scale));
        Microphone::peakValues[i] = round(0.0 + round(Microphone::dlt2Values[i] * Microphone::scale));
    }

    uint8_t numOutliers0015U = 0;
    uint8_t numOutliers0015L = 0;
    for (int i = 0; i < AUDIO________NUM_BANDS; i++) {
        if (Microphone::lineValues[i] > 8) {
            numOutliers0015U++;
        } else if (Microphone::bandValues[i] < Microphone::basis) {
            numOutliers0015L++;
        }
    }

    if (numOutliers0015U > 0) {
        Microphone::scale *= 0.99;
        // Serial.print("decrease scale: ");
        // Serial.println(String(Microphone::scale, 5));
    } else {
        if (Microphone::scale < 0.01) {
            Microphone::scale *= 1.01;
            // Serial.print("increase scale: ");
            // Serial.println(String(Microphone::scale, 5));
        } else {
            // Serial.println("increase scale: skip");
        }
    }

    if (numOutliers0015L > 2) {
        Microphone::basis *= 0.99;
        // Serial.print("decrease scale: ");
        // Serial.println(String(Microphone::scale, 5));
    } else {
        Microphone::basis *= 1.01;
    }
}

bool Microphone::depower() {
    // nothing
    return true;
}