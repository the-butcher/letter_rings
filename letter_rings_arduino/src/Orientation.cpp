#include "Orientation.h"

Adafruit_BNO055 Orientation::baseSensor(55, 0x28);
vector________t Orientation::orientation = {0, 0, 0};
vector________t Orientation::gyroscope = {0, 0, 0};
acceleration__t Orientation::acceleration;
double Orientation::coefficient;

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
    sensors_event_t accelerationData;

    Orientation::baseSensor.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
    Orientation::baseSensor.getEvent(&gyroscopeData, Adafruit_BNO055::VECTOR_GYROSCOPE);
    Orientation::baseSensor.getEvent(&accelerationData, Adafruit_BNO055::VECTOR_LINEARACCEL);
    Orientation::orientation = {orientationData.orientation.x, orientationData.orientation.y, orientationData.orientation.z};
    Orientation::gyroscope = {gyroscopeData.gyro.x, gyroscopeData.gyro.y, gyroscopeData.gyro.z};

    for (uint8_t i = 0; i < ACCELERATION___SAMPLES - 1; i++) {
        Orientation::acceleration.values[i] = Orientation::acceleration.values[i + 1];
    }
    float accelerationMagnitude = sqrt(pow(accelerationData.acceleration.x, 2) + pow(accelerationData.acceleration.y, 2) + pow(accelerationData.acceleration.z, 2));

    Orientation::acceleration.values[ACCELERATION___SAMPLES - 1] = accelerationMagnitude;

    return true;
}

vector________t Orientation::getOrientation() {
    return Orientation::orientation;
}

vector________t Orientation::getGyroscope() {
    return Orientation::gyroscope;
}

// https://unacademy.com/content/jee/study-material/mathematics/pearson-correlation-coefficient/
void Orientation::calculateCoefficient(acceleration__t accelA, acceleration__t accelB) {

    double sumA = 0;
    double sumB = 0;
    double sumAB = 0;
    double sumAS = 0;
    double sumBS = 0;
    for (int i = 0; i < ACCELERATION___SAMPLES; i++) {
        sumA += accelA.values[i];
        sumB += accelB.values[i];
        sumAB += accelA.values[i] * accelB.values[i];
        sumAS += accelA.values[i] * accelA.values[i];
        sumBS += accelB.values[i] * accelB.values[i];
    }
    double r0 = ACCELERATION___SAMPLES * sumAB - sumA * sumB;
    double r1 = sqrt((ACCELERATION___SAMPLES * sumAS - sumA * sumA) * (ACCELERATION___SAMPLES * sumBS - sumB * sumB));
    double coefficient = r0 / r1;

    if (!isnan(coefficient)) {
        double f = coefficient > Orientation::coefficient ? 0.10 : 0.01;                  // the speed at which the low pass filter adapts
        Orientation::coefficient = Orientation::coefficient * (1 - f) + coefficient * f;  // low pass
    }
}