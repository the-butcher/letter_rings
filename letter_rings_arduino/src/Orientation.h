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

   public:
    static bool hasBegun;
    static bool readval();
    static vector________t getOrientation();
    static vector________t getGyroscope();  // radians per seconds
    static bool powerup();
    static bool depower();
};

#endif