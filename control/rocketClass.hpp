#include <MatrixMath.h>
#include <math.h>
#include "flightplan.hpp"
#include <Wire.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h>

#ifndef _ROCKET_HPP_
#define _ROCKET_HPP_
#define numOfCParams 4
#define commsDevice 19
#define fpacc 5
#define numBytes 64

class rocket {
public:
    rocket();
    ~rocket(){};
    int fillModel(int, int);
    int updateSensorData(Adafruit_BNO055&, Adafruit_BMP280&);
    int sendDataComms(int);
    int updateRotMatrix();
    float getSpeed();
    float getSpeedSq();
    float getRoll();
    float getRollRate();
    float getPitch();
    float getz() {return z;};
    float* getQ() {return Q;};

private:
    // Orientation Data
    float Q[4] {0,0,0,0};
    float vQ[4] {0,0,0,0};
    float aQ[4] {0,0,0,0};

    float pitch;
    float roll;
    float rollRate;

    long lastUpdate;
    int deltaT;

    float R[9]{0,0,0,
               0,0,0,
               0,0,0};     // rotation matrix, stored beause it's frequently used.
    float up[3]{0,0,1};    // points "up" in the ground frame.    Placeholder is 0,0,1
    float north[3]{1,0,0}; // points "north" in the ground plane. Placeholder is 1,0,0 or 0,1,0

    // Location Data and Trajectory
    // All values should be in ground frame.

    float z;
    float oldZ;
    float zV;

    bool rollUp2Date;
    bool pitchUp2Date;
    bool rollMatrixUp2Date;
    bool speedUp2Date;

    float omega;
    float calibrationPressure;
    float moi;
    flightplan plan;
};

#endif
