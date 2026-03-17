#include "Orientation.h"

Adafruit_BNO055 Orientation::baseSensor(55, 0x28);
vector________t Orientation::orientation = { 0, 0, 0 };
acceleration__t Orientation::accelA;
acceleration__t Orientation::accelB;
double Orientation::coefficient;
double Orientation::coefficientThreshold = 0.8;

bool Orientation::powered = false;

bool Orientation::powerup() {
    Orientation::powered = Orientation::baseSensor.begin();
    return Orientation::powered;
}

bool Orientation::depower() {
    // nothing
    return true;
}

/**
 * consumes ~ 5ms for orientation, gyroscope, linearaccel
 * consumes ~ 1.8ms for acceleration only
 */
bool Orientation::read() {

    sensors_event_t accelerationData;
    sensors_event_t orientationData;

    Orientation::baseSensor.getEvent(&accelerationData, Adafruit_BNO055::VECTOR_LINEARACCEL);
    Orientation::baseSensor.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);

    // move all accel values up
    for (uint8_t i = 0; i < ACCELERATION___SAMPLES - 1; i++) {
        Orientation::accelA.values[i] = Orientation::accelA.values[i + 1];
    }
    // calculate last value and put it
    float accelerationMagnitude = sqrt(pow(accelerationData.acceleration.x, 2) + pow(accelerationData.acceleration.y, 2) + pow(accelerationData.acceleration.z, 2));
    Orientation::accelA.values[ACCELERATION___SAMPLES - 1] = accelerationMagnitude;

    Orientation::orientation = { orientationData.orientation.x, orientationData.orientation.y, orientationData.orientation.z };

    return true;

}

vector________t Orientation::getOrientation() {
    return Orientation::orientation;
}

acceleration__t Orientation::getAccelA() {
    return Orientation::accelA;
}

acceleration__t Orientation::getAccelB() {
    return Orientation::accelB;
}

void Orientation::setAccelAMillisWait(int64_t millisWaitA) {
    Orientation::accelA.millisWait = millisWaitA;
}

void Orientation::setAccelB(acceleration__t accelB) {
    for (uint8_t i = 0; i < ACCELERATION___SAMPLES; i++) {
        Orientation::accelB.values[i] = accelB.values[i];
    }
    Orientation::accelB.millisWait = accelB.millisWait;
    Orientation::calculateCoefficient();
}

// https://unacademy.com/content/jee/study-material/mathematics/pearson-correlation-coefficient/
void Orientation::calculateCoefficient() {

    double sumA = 0;
    double sumB = 0;
    double sumAB = 0;
    double sumAS = 0;
    double sumBS = 0;
    for (uint8_t i = 0; i < ACCELERATION___SAMPLES; i++) {
        sumA += Orientation::accelA.values[i];
        sumB += Orientation::accelB.values[i];
        sumAB += Orientation::accelA.values[i] * Orientation::accelB.values[i];
        sumAS += Orientation::accelA.values[i] * Orientation::accelA.values[i];
        sumBS += Orientation::accelB.values[i] * Orientation::accelB.values[i];
    }
    double r0 = ACCELERATION___SAMPLES * sumAB - sumA * sumB;
    double r1 = sqrt((ACCELERATION___SAMPLES * sumAS - sumA * sumA) * (ACCELERATION___SAMPLES * sumBS - sumB * sumB));
    double coefficient = r0 / r1;

    if (!isnan(coefficient)) {
        double f = coefficient > Orientation::coefficient ? 0.10 : 0.01;                  // the speed at which the low pass filter adapts
        Orientation::coefficient = Orientation::coefficient * (1 - f) + coefficient * f;  // low pass
    }

}

double Orientation::getCoefficient() {
    return Orientation::coefficient;
}

double Orientation::getCoefficientThreshold() {
    return Orientation::coefficientThreshold;
}

bool Orientation::setCoefficientThreshold(double coefficientThreshold) {
    if (coefficientThreshold >= COEFFICIENT_THRES__MIN && coefficientThreshold <= COEFFICIENT_THRES__MAX) {
        Orientation::coefficientThreshold = coefficientThreshold;
        return true;
    } else {
        return false;
    }
}

bool Orientation::isAboveCoefficientThreshold() {
    return Orientation::coefficient >= Orientation::coefficientThreshold;
}

bool Orientation::isAboveSignificantThreshold() {
    for (uint8_t i = ACCELERATION___SAMPLES / 2; i < ACCELERATION___SAMPLES; i++) {
        if (Orientation::accelA.values[i] > ACCELERATION_SIG_THRES) {
            return true;
        }
    }
    return false;
}