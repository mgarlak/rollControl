#include "rocketClass.hpp"

int rocket::updateSensorData(Adafruit_BMP280 bmp, Adafruit_BNO055 orient /*Other Sensors*/){
	
}

double rocket::getSpeed(){
    return sqrt(getSpeed());
}
double rocket::getSpeedSq(){
    return xV*xV+yV*yV+zV*zV;
}