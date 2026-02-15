#ifndef Microphone_h
#define Microphone_h

#include <Arduino.h>
#include <arduinoFFT.h>
#include <curveFitting.h>

#include "Define.h"

class Microphone {
   private:
    /**
     * fast fourier transform instance
     */
    static ArduinoFFT<double> FFT;
    static unsigned int sampling_period_us;
    static double vReal[AUDIO__________SAMPLES];
    static double vImag[AUDIO__________SAMPLES];
    static unsigned long newTime;
    /**
     * min/max indices to associate samples with bands, a simple quadratic curve built in powerup
     */
    static int buckValues[AUDIO________NUM_BANDS];
    static double dlt1Values[AUDIO________NUM_BANDS];
    static double dlt2Values[AUDIO________NUM_BANDS];

    static double fitFAverag;
    static double fitXValues[AUDIO________NUM_BANDS];  // a list of ascending double values
    static double fitYValues[AUDIO________NUM_BANDS];  // bandValues as double values
    static double coefValues[AUDIO________NUM_ORDER + 1];
    static double curvValues[AUDIO________NUM_BANDS];  // parabolic shape, visual only

   public:
    static double fitFValues[AUDIO________NUM_BANDS];  // filtered fit values, public to be drawable in display
    static int bandScaled[AUDIO________NUM_BANDS];

    static double scale;
    static double basis;
    static double slope;
    static uint64_t signal;
    /**
     * line lengths by band as actually to be drawn on the led-matrices, length 1-8
     */
    static int lineValues[AUDIO________NUM_BANDS];  // the values actually shown
    static int peakValues[AUDIO________NUM_BANDS];
    static int bandValues[AUDIO________NUM_BANDS];  // actual band values direclty after sampling
    /**
     * how fast to decay the lines (not the peak, which is hardcoded - should it?)
     */
    static uint8_t decay;
    static bool powerup();
    static void sample();
    static bool depower();
};

#endif