#include "rocketClass.hpp"
#include <Servo.h>
#define commsRst 6
#define controlRst 7
#define servoPin 8
int flightMode;
rocket hprcRock;
Adafruit_BMP280 bmp;
Adafruit_BNO055 orient = Adafruit_BNO055(55);
Servo ailerons;
bool wireFlag = false;

void setup() {
    serialDump();
    pinMode(commsRst, OUTPUT);
    pinMode(commsRst, HIGH);
    ailerons.attach(servoPin);
    Serial.begin(9600);
    Wire.begin(75);

    Serial.print(F("Initializing SD Card..."));
    Wire.requestFrom(commsDevice, 1);
    char chk = Wire.read();
    for (int i = 0; i < 2; ++i){
        if (i == 0 && chk != '1'){
            Serial.print(F("Syncronizing Comms Device..."));
            resetDev(commsRst);
            delay(1500);
            Wire.requestFrom(commsDevice, 1);
            chk = Wire.read();
        }
        else if (i == 1 && chk != '1') {
            Serial.println(F("SD INIT ERROR"));
            while (1);
        }
        else break;
    }
    Serial.println(F("SD Card initialization successful."));

    Serial.print(F("Parsing Configuration File..."));
    if (hprcRock.fillModel(fpacc, commsDevice) != 0){
        Serial.print(F("CONFIG PARSE ERROR"));
        while (1);
    }
    else Serial.println(F("Config File Parsed"));

    if (!orient.begin()){
        Serial.print(F("BNO FAILURE"));
        while(1);
    }
    if (!bmp.begin()){  
        Serial.println(F("BMP FAILURE"));
        while (1);
    }
    Serial.println(F("Sensors Initilized"));
    flightMode=0;
}

void loop() {
    Serial.println(F("IN LOOP"));
    //any code that needs to run every loop regardless of flightMode.
    if (hprcRock.updateSensorData(orient, bmp) == 0){

    }
    //Send Sensor Data for logging
//    hprcRock.logData();
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

void serialDump(){
    while (Serial.available()){
        char d = Serial.read();
    }
}

void resetDev(int pin){
    digitalWrite(pin, LOW);
    delay(25);
    digitalWrite(pin, HIGH);
}
