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

/*#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <Wire.h>

#define BNO055_SAMPLERATE_DELAY_MS (1)
Adafruit_BNO055 orient = Adafruit_BNO055(55);
Adafruit_BMP280 alt;

void displaySensorDetails(Adafruit_BNO055&);

void setup() {
	Serial.begin(9600);
	Serial.println("Orientation Sensor Test"); Serial.println("");

	// Initialise the sensor
	if(!orient.begin())
	{
		// There was a problem detecting the BNO055 ... check your connections
		Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
		while(1);
	}

	delay(1000);

	// Display some basic information on this sensor
	displaySensorDetails(orient);

	orient.setExtCrystalUse(true);

	Serial.println(F("BMP280 test"));
	
	if (!alt.begin()) {  
		Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
		while (1);
	}
	
}


void loop() {
	// Get a new sensor event
	sensors_event_t event;
	orient.getEvent(&event);

	// Display the floating point data
	Serial.print("X: ");
	Serial.print(event.orientation.x, 4);
	Serial.print("\tY: ");
	Serial.print(event.orientation.y, 4);
	Serial.print("\tZ: ");
	Serial.print(event.orientation.z, 4);

	//PRINTING SENSOR DETAILS
	printBNO055(orient);
	
	Serial.println("");
	Serial.print(F("Temperature = "));
	Serial.print(alt.readTemperature());
	Serial.println(" *C");
		
	Serial.print(F("Pressure = "));
	Serial.print(alt.readPressure());
	Serial.println(" Pa");

	Serial.print(F("Approx altitude = "));
	Serial.print(alt.readAltitude(1013.25)); // this should be adjusted to your local forcase
	Serial.println(" m");
	
	// New line for the next sample
	Serial.println("");

	// Wait the specified delay before requesting nex data
	delay(BNO055_SAMPLERATE_DELAY_MS);
}

void printBNO055(Adafruit_BNO055 &bno){
	uint8_t system, gyro, accel, mag;
	bno.getCalibration(&system, &gyro, &accel, &mag);

	Serial.print('\t');
	if (!system){
		Serial.print("! ");
	}

	Serial.print("SYS:");
	Serial.print(system, DEC);
	Serial.print(" GYRO:");
	Serial.print(gyro, DEC);
	Serial.print(" ACCEL:");
	Serial.print(accel, DEC);
	Serial.print(" MAG:");
	Serial.print(mag, DEC);
}

void displaySensorDetails(Adafruit_BNO055 &bno){
	sensor_t sensor;
	bno.getSensor(&sensor);
	Serial.println("------------------------------------");
	Serial.print  ("Sensor:       "); Serial.println(sensor.name);
	Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
	Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
	Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" xxx");
	Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" xxx");
	Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" xxx");
	Serial.println("------------------------------------");
	Serial.println("");
	delay(500);
}*/