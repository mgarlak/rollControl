#include "utility.hpp"
#include <SPI.h>
#include <SD.h>
#include <Wire.h>

#define configFile "ROCKETCF.TXT"
#define flightLog "flogger1.txt"
#define sdPin 10
#define fpacc 5
#define controlDevice 75

File configf;
File logger;
int cmdSqnc = 0;
char data[22];
char* fp2 = nullptr;
bool newConfig = true;

void setup(){
    SD.begin(sdPin);
    Serial.begin(57600);
    Wire.begin(19);

    Serial.print(F("Waiting For Instruction..."));
    Wire.onRequest(requestHandler);
    Wire.onReceive(receiveHandler);
}

void loop(){
    if (newConfig){
        while (Serial.available()){
            fp2 = caAppend(fp2, Serial.read());
        }
        if (fp2 != nullptr){
            Serial.write(fp2);
            delete[] fp2;
            newConfig = false;
        }
    }
}

void requestHandler(){
    Serial.println("IN REQUEST");
    switch (cmdSqnc){
        case 0: ackSD(); break;
        case 1: sendParam(); break;
        default: {}
    }
}

void receiveHandler(int bytesReceived){
    Serial.println("IN RECEIVE");
    for (int i = 0; Wire.available() && i < 22; ++i){
        data[i] = Wire.read();
    }
    Serial.println(str);
    logSD(str);
    transmitXbee(str);
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