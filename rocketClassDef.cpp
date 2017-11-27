#include "rocketClass.hpp"

double rocket::getSpeed(){
    return sqrt(getSpeedSq());
}
double rocket::getSpeedSq(){
    return xV*xV+yV*yV+zV*zV;
}

int rocket::updateSensorData(Adafruit_BMP280 bmp, Adafruit_BNO055 orient){
  imu::Quaternion quat = orient.getQuat();
  Q[0] = quat.x();
  Q[1] = quat.y();
  Q[2] = quat.z();
  Q[3] = quat.w();

  z = bmp.readAltitude(1013.25 /*HARDCODED, WE'LL CHANGE LATER*/);
}

double rocket::getPitch(){
    if (!pitchUp2Date){
        pitch=/*(right angle) - arccos(The dot product of the rocket-frame z unit vector and the ground frame z unit vector)*/;
    }
    return pitch;
}
double rocket::getRoll(){
    if(!rollUp2Date){
        roll=/*angle between the projection of the x unit vector in the rocket frame on the ground frame xy plane and the ground frame x direction*/;
    }
    return roll;
}

double rocket::getRollRate(){
    //Should be nearly identical to get roll, except using vQ instead of Q. 
}