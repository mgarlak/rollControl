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