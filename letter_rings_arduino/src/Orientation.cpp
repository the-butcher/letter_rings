#include "Orientation.h"

Adafruit_BNO055 Orientation::baseSensor(55, 0x28);
vector________t Orientation::orientation = {0, 0, 0};
vector________t Orientation::gyroscope = {0, 0, 0};

bool Orientation::hasBegun = false;

bool Orientation::powerup() {
    Orientation::hasBegun = Orientation::baseSensor.begin();
    return Orientation::hasBegun;
}

bool Orientation::depower() {
    // nothing
    return true;
}

bool Orientation::readval() {
    sensors_event_t orientationData;
    sensors_event_t gyroscopeData;
    Orientation::baseSensor.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
    Orientation::baseSensor.getEvent(&gyroscopeData, Adafruit_BNO055::VECTOR_GYROSCOPE);
    Orientation::orientation = {orientationData.orientation.x, orientationData.orientation.y, orientationData.orientation.z};
    Orientation::gyroscope = {gyroscopeData.gyro.x, gyroscopeData.gyro.y, gyroscopeData.gyro.z};
    return true;
}

vector________t Orientation::getOrientation() {
    return Orientation::orientation;
}

vector________t Orientation::getGyroscope() {
    return Orientation::gyroscope;
}