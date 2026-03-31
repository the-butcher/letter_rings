#include "Orientation.h"

Adafruit_BNO055 Orientation::baseSensor(55, 0x28);
vector________t Orientation::orientation = { 0, 0, 0 };
magnitudes___t Orientation::magnitudesA;
magnitudes___t Orientation::magnitudesB;
acceleration_t Orientation::accelerationsA;
double Orientation::coefP;
double Orientation::coefPThreshold = 0.80;

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
        Orientation::magnitudesA.values[i] = Orientation::magnitudesA.values[i + 1];
        Orientation::accelerationsA.valuesX[i] = Orientation::accelerationsA.valuesX[i + 1];
        Orientation::accelerationsA.valuesY[i] = Orientation::accelerationsA.valuesY[i + 1];
        Orientation::accelerationsA.valuesZ[i] = Orientation::accelerationsA.valuesZ[i + 1];
    }

    // calculate last value and put it in last position
    float accelerationMagnitude = sqrt(pow(accelerationData.acceleration.x, 2) + pow(accelerationData.acceleration.y, 2) + pow(accelerationData.acceleration.z, 2));
    Orientation::magnitudesA.values[ACCELERATION___SAMPLES - 1] = accelerationMagnitude;
    Orientation::accelerationsA.valuesX[ACCELERATION___SAMPLES - 1] = accelerationData.acceleration.x;
    Orientation::accelerationsA.valuesY[ACCELERATION___SAMPLES - 1] = accelerationData.acceleration.y;
    Orientation::accelerationsA.valuesZ[ACCELERATION___SAMPLES - 1] = accelerationData.acceleration.z;

    Orientation::orientation = { orientationData.orientation.x, orientationData.orientation.y, orientationData.orientation.z };

    return true;

}

acceleration_t Orientation::getAccelerationsA() {
    return Orientation::accelerationsA;
}

vector________t Orientation::getOrientation() {
    return Orientation::orientation;
}

magnitudes___t Orientation::getMagnitudesA() {
    return Orientation::magnitudesA;
}

magnitudes___t Orientation::getMagnitudesB() {
    return Orientation::magnitudesB;
}

void Orientation::setMillisWaitA(int64_t millisWaitA) {
    Orientation::magnitudesA.millisWait = millisWaitA;
}

void Orientation::setMagnitudesB(magnitudes___t accelB) {
    for (uint8_t i = 0; i < ACCELERATION___SAMPLES; i++) {
        Orientation::magnitudesB.values[i] = accelB.values[i];
    }
    Orientation::magnitudesB.millisWait = accelB.millisWait;
    Orientation::calculateCoefP();
}

// https://unacademy.com/content/jee/study-material/mathematics/pearson-correlation-coefficient/
void Orientation::calculateCoefP() {

    double coefP = Orientation::calculateCoefficient(Orientation::magnitudesA.values, Orientation::magnitudesB.values);
    if (!isnan(coefP)) {
        double f = coefP > Orientation::coefP ? 0.10 : 0.01;                  // the speed at which the low pass filter adapts
        Orientation::coefP = Orientation::coefP * (1 - f) + coefP * f;  // low pass
    }

}

double Orientation::calculateCoefficient(float valuesA[ACCELERATION___SAMPLES], float valuesB[ACCELERATION___SAMPLES], uint8_t count) {

    double sumA = 0;
    double sumB = 0;
    double sumAB = 0;
    double sumAS = 0;
    double sumBS = 0;
    for (uint8_t i = 0; i < count; i++) {
        sumA += valuesA[i];
        sumB += valuesB[i];
        sumAB += valuesA[i] * valuesB[i];
        sumAS += valuesA[i] * valuesA[i];
        sumBS += valuesB[i] * valuesB[i];
    }
    double r0 = count * sumAB - sumA * sumB;
    double r1 = sqrt((count * sumAS - sumA * sumA) * (count * sumBS - sumB * sumB));
    return r0 / r1;

}

double Orientation::getCoefP() {
    return Orientation::coefP;
}

double Orientation::getCoefPThreshold() {
    return Orientation::coefPThreshold;
}

bool Orientation::setCoefPThreshold(double coefPThreshold) {
    if (coefPThreshold >= COEFF________THRES_MIN && coefPThreshold <= COEFF________THRES_MAX) {
        Orientation::coefPThreshold = coefPThreshold;
        return true;
    } else {
        return false;
    }
}

bool Orientation::isAboveCoefPThreshold() {
    return Orientation::coefP >= Orientation::coefPThreshold;
}

bool Orientation::isAboveSignificantThreshold(float threshold, uint8_t fromIndex, uint8_t toIndex) {
    for (uint8_t i = fromIndex; i < toIndex; i++) { // ACCELERATION___SAMPLES / 2
        if (Orientation::magnitudesA.values[i] > threshold) {
            return true;
        }
    }
    return false;
}