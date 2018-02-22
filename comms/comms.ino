#include "utillity.hpp"
#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>

#define configFile "ROCKETCF.TXT"
#define flightLog "flogger1.txt"
#define sdPin 10
#define fpacc 5
#define controlDevice 75

SoftwareSerial Xbee(2,3); //Rx, Tx
File configf;
File logger;
int cmdSqnc = 0;
bool wireFlag = false;

void setup(){
  	SD.begin(sdPin);
	Xbee.begin(9600);
	Serial.begin(9600);
	Wire.begin(19);

 	Serial.print(F("Waiting For Instruction..."));
  	Wire.onRequest(requestHandler);
}

void loop(){
    // read data
    Wire.requestFrom(controlDevice, 21);
    char data[22];
    data[22] = '\0';
    int i = 0;
    while (Wire.available() && i < 22){
        data[i] = wire.read();
        ++i;
    }

    // Now convert
    /*char qChar[5][4];
    i = 0;
    int j = 0;
    int k = 0;
    while (i < 5){
        if (j%4 == 0) {j = 0; ++i;}
        qChar[i][j] = data[k];
        ++j; ++k;
    }
    */
    // log and transmit
    logSD(data);
    transmitXbee(data);
}

void requestHandler(){
    Serial.print("IN REQUEST HANDLER");
    switch (cmdSqnc){
        case 0: ackSD(); break;
        case 1: sendParam(); break;
        default: {}
    }
}

void ackSD(){
    Serial.print(F("Initilizing SD card..."));
    configf = SD.open(configFile);
    if (configf){
        sendAck(); 
        Serial.println(F("SD Card Init Success!"));
        ++cmdSqnc;
    }
    else Serial.println(F("SD Card Init Failure!"));
}

void sendParam(){
    Serial.print(F("Parsing a parameter..."));
	if (configf){
		char* str = nullptr;
		while (1){
			char ch = configf.read();
            if (ch == -1) {
                Serial.println(F("Done Parsing"));
                ++cmdSqnc; 
                break;
            }
			else if (ch == '\n'){
                Serial.print(F("Sending Parameter..."));
                Wire.write(str);
                delete[] str;
                str = nullptr;
                break;
			}
			else if (isFpVital(ch)){str=caAppend(str, ch);}
		}
	}
}

/*Implement a function to move forward with the next cmdSqnc*/

void sendAck(){
    Wire.write('1');
}

void logSD(char* str){
    if (!logger){ logger = SD.open(flightLog); }
    logger.println(str, HEX);
}

void transmitXbee(char* str){
    Xbee.print(str, HEX);
}