#include "rocketClass.hpp"
#include "rocketClassDef.cpp"

int flightMode;
rocket hprcRock;

void setup() {
	// put your setup code here, to run once:
	// Initilize hprcRock here.
	flightMode=0;
}



void loop() {
	//any code that needs to run every loop regardless of flightMode.

	if (hprcRock.updateSensorData() < 0){
		double* qt = getQ();

		Serial.println("PRINTING QUATERION:");
		Serial.println(qt[0]);
		Serial.println(qt[1]);
		Serial.println(qt[2]);
		Serial.println(qt[3]);

		Serial.println("PRINTING ALTITUDE:");
		Serial.println(getz());

	}

	switch (flightMode){
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
			break;
	}
}