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
private:
	// Orientation Data
	double roll;
	double pitch;
	double yaw;
	double rollV;
	double pitchV;
	double yawV;
	double rollA;
	double pitchA;
	double yawA;

	// Location Data and Trajectory
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