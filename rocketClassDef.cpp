#include "rocketClass.hpp"

rocket::rocket(){
	// Orientation Data
	Q = NULL;
	vQ = NULL;
	aQ = NULL;
	pitch = 0;
	roll = 0;
	rollRate = 0;
	R = NULL // rotation matrix, stored beause it's frequently used.
	// Location Data and Trajectory
	// All values should be in ground frame.
	x = 0;   // Position x
	y = 0;   // Position y
	z = 0;   // Altitude
	xV = 0;  // Change in x
	yV = 0;  // Change in y
	zV = 0;  // Change in Altitude
	xA = 0;  
	yA = 0;  
	zA = 0;  
	rollUp2Date = false;
	pitchUp2Date = false;
	Adafruit_BMP280 bmp;
	Adafruit_BNO055 orient = Adafruit_BNO055(55);
}

double rocket::getSpeed(){
	return sqrt(getSpeedSq());
}
double rocket::getSpeedSq(){
	return xV*xV+yV*yV+zV*zV;
}

int rocket::updateSensorData(){
	imu::Quaternion quat = orient.getQuat();
	Q[0] = quat.x();
	Q[1] = quat.y();
	Q[2] = quat.z();
	Q[3] = quat.w();
	z = bmp.readAltitude(1013.25 /*HARDCODED, WE'LL CHANGE LATER*/);
	pitchUp2Date = false;
	rollUp2Date = false;
	return 0;
}

double rocket::getPitch(){
	if (!pitchUp2Date){
		pitch=/*(right angle) - arccos(The dot product of the rocket-frame z unit vector and the ground frame z unit vector)*/;
	}

	pitchUp2Date = true;
	return pitch;
}
double rocket::getRoll(){
	if(!rollUp2Date){
		roll=/*angle between the projection of the x unit vector in the rocket frame on the ground frame xy plane and the ground frame x direction*/;
	}

	rollUp2Date = true;
	return roll;
}

double rocket::getRollRate(){
	//Should be nearly identical to get roll, except using vQ instead of Q. 
}