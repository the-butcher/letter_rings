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
    static vector________t gyroscope;
    static acceleration__t accelA;
    static acceleration__t accelB;
    static double coefficient;
    static double coefficientThreshold;

public:
    static bool hasBegun;
    static bool read();
    static vector________t getOrientation();
    static vector________t getGyroscope();  // radians per seconds
    static bool powerup();
    static bool depower();
    static acceleration__t getAccelA();
    static acceleration__t getAccelB();
    static void setAccelB(acceleration__t accelB);
    static void calculateCoefficient();
    static double getCoefficient();
    static double getCoefficientThreshold();
    static bool setCoefficientThreshold(double coefficientThreshold);
    static bool isAboveCoefficientThreshold();
};

#endif