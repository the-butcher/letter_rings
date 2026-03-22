#ifndef Orientation_h
#define Orientation_h

#include <Adafruit_BNO055.h>
#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <Wire.h>
#include <utility/imumaths.h>

#include "Define.h"

/**
 * orientation.x has axis normal to sensor plane
 * orientation.y has axis on sensor plane, perpendicular to cable
 * orientation.z has axis sensor plane, along cable
 */
class Orientation {
private:

    static Adafruit_BNO055 baseSensor;
    static vector________t orientation;

    /**
     * own magnitude values
     */
    static magnitudes___t magnitudesA;

    /**
     * other magnitude values
     */
    static magnitudes___t magnitudesB;

    /**
     * most recent acceleration values
     */
    static acceleration_t accelerationsA;

    static double coefP;
    static double coefPThreshold;

    static double coefGThreshold;

public:

    static bool read();
    static vector________t getOrientation();

    static magnitudes___t getMagnitudesA();
    static magnitudes___t getMagnitudesB();

    static acceleration_t getAccelerationsA();

    static void setMillisWaitA(int64_t millisWaitA);

    static void setMagnitudesB(magnitudes___t magnitudesB);

    /**
     * calculate a new coefP based on the current magnitudesA and magnitudesB values
     */
    static void calculateCoefP();

    static double calculateCoefficient(float valuesA[ACCELERATION___SAMPLES], float valuesB[ACCELERATION___SAMPLES], uint8_t count = ACCELERATION___SAMPLES);

    static bool matchGesture(acceleration_t gesture, float threshold = 1.5);

    /**
     * get the current coefG (gesture) value
     */
    static double getCoefGThreshold();

    /**
     * set a new coefP threshold value
     * @return true if the value could be set (was within min/max values), false otherwise
     */
    static bool setCoefGThreshold(double coefGThreshold);

    /**
     * get the current coefP value
     */
    static double getCoefP();

    /**
     * get the current coefP threshold
     */
    static double getCoefPThreshold();

    /**
     * set a new coefP threshold value
     * @return true if the value could be set (was within min/max values), false otherwise
     */
    static bool setCoefPThreshold(double coefPThreshold);

    /**
     * check if the current coefP is above the current coefP threshold
     */
    static bool isAboveCoefPThreshold();

    /**
     * check if one of the recent acceleration values (in magnitudesA) is above the significant threshold (ACCELERATION_SIG_THRES)
     */
    static bool isAboveSignificantThreshold(float threshold, uint8_t fromIndex = ACCELERATION___SAMPLES / 2, uint8_t toIndex = ACCELERATION___SAMPLES);

    static bool powered;
    static bool powerup();
    static bool depower();

};

#endif