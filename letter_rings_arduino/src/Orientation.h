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
 * orientation.y has axis perpendicular to cable
 * orientation.z has axis along cable
 */
class Orientation {
private:

    static Adafruit_BNO055 baseSensor;
    static vector________t orientation;

    /**
     * own acceleration values
     */
    static acceleration__t accelA;
    /**
     * other acceleration values
     */
    static acceleration__t accelB;

    static double coefficient;
    static double coefficientThreshold;

public:

    static bool read();
    static vector________t getOrientation();

    static acceleration__t getAccelA();
    static acceleration__t getAccelB();

    static void setAccelAMillisWait(int64_t millisWaitA);

    static void setAccelB(acceleration__t accelB);

    /**
     * calculate a new coefficent based on the current accelA and accelB values
     */
    static void calculateCoefficient();

    /**
     * get the current coefficient value
     */
    static double getCoefficient();

    /**
     * get the current coefficient threshold
     */
    static double getCoefficientThreshold();

    /**
     * set a new coffeficient threshold value
     * @return true if the value could be set (was within min/max values), false otherwise
     */
    static bool setCoefficientThreshold(double coefficientThreshold);

    /**
     * check if the current coefficient is above the current coefficient threshold
     */
    static bool isAboveCoefficientThreshold();

    /**
     * check if one of the recent acceleration values (in accelA) is above the significant threshold (ACCELERATION_SIG_THRES)
     */
    static bool isAboveSignificantThreshold();

    static bool powered;
    static bool powerup();
    static bool depower();

};

#endif