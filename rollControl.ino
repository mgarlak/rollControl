#include "rocketClass.hpp"
<<<<<<< HEAD
#include "rocketClassDef.cpp"
#include "flightplan.hpp"
=======
>>>>>>> a77f2ac9e5ae5bc8f37119f07d1f69b6b20ec7f9

int flightMode;
Adafruit_BMP280 bmp;
Adafruit_BNO055 orient;

void setup() {
  // put your setup code here, to run once:
  flightMode=0;
}

double goalTorque(const rocket&, const flightplan &);
double detlaTorque(double tau, const rocket&);
double finAngle(double tau, const rocket &);

void loop() {
  //any code that needs to run every loop regardless of flightMode.
	if (updateSensorData(bmp, orient) < 0){
		double* qt = getQ();

    Serial.println("PRINTING QUATERION:");
    Serial.println(qt[0]);
    Serial.println(qt[1]);
    Serial.println(qt[2]);
    Serial.println(qt[3]);

    Serial.println("PRINTING ALTITUDE:");
    Serial.println(getz())

	}
  case(flightMode){
    case 0 : 
      //On the ground
      break;
    case 1:
      //boost phase
      break;
    case 2:
      //Coast phase, where we control roll
      break;
    case 3:
      //Decent phase, initial
      break;
    case 4:
      //Decent phase, after main chute deply
      break;
    case 5:
      //on ground
  }
}