#include <Adafruit_BNO055.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h>

#ifndef _ROCKET_HPP_
#define _ROCKET_HPP_

class rocket
{
public:
  rocket();
  ~rocket();
  int updateSensorData(Adafruit_BMP280, Adafruit_BNO055 /*Other Sensors*/);
  int logData();
  int updateRotMatrix();
  double getSpeed();
  double getSpeedSq();
  double getRoll();
  double getRollRate();
  double getz() {return z;};
  double* getQ() {return Q;};
private:
  // Orientation Data
  double Q[4];
  double vQ[4];
  double aQ[4];

  double pitch;
  double roll;
  double rollRate;

  float R[9]; // rotation matrix, stored beause it's frequently used.

  // Location Data and Trajectory
  // All values should be in ground frame.
  double x;
  double y;
  double z;
  double xV;
  double yV;
  double zV;
  double xA;
  double yA;
  double zA;
};

#endif