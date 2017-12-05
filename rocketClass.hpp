#include <Adafruit_BNO055.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h>

#ifndef _ROCKET_HPP_
#define _ROCKET_HPP_

class rocket {
public:
	rocket();
	~rocket();
	int updateSensorData();
	int logData();
	int updateRotMatrix();
	double getSpeed();
	double getSpeedSq();
	double getRoll();
	double getRollRate();
	double getPitch();
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
	double x;   // Position x
	double y;   // Position y
	double z;   // Altitude
	double xV;  // Change in x
	double yV;  // Change in y
	double zV;  // Change in Altitude
	double xA;  
	double yA;  
	double zA;  

	bool rollUp2Date;
	bool pitchUp2Date;

	Adafruit_BMP280 bmp;
	Adafruit_BNO055 orient;
};

#endif