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
bool keepListening = true;

void setup(){
    serialDump();
    SD.begin(sdPin);
    Serial.begin(57600);
    Wire.begin(19);

    Serial.print(F("Waiting For Instruction..."));
    Wire.onRequest(requestHandler);
    Wire.onReceive(receiveHandler);
}

void loop(){
    while (keepListening){
        char inc = Serial.read();
        delay(1);
        if (inc == '\0'){ // entire FP received
            int i = 0;
            while(fp2[i] != '\0'){
                Wire.write(fp2[i]);
                ++i;
            }
            Wire.requestFrom(controlDevice, 1);
            if (Wire.read() == '0'){
                Serial.println(F("VALID FLIGHT PLAN"));
                keepListening = false;
                break;
            }
            else {
                Serial.println(F("INVALID FLIGHT PLAN. TRY AGAIN"));
                delete[] fp2;
                fp2 = nullptr;
                continue;
            }
        }
        else if (inc == -1) continue;
        else fp2 = caAppend(fp2, inc);
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
    Serial.println(F("IN RECEIVE"));
    for (int i = 0; Wire.available() && i < 22; ++i){
        data[i] = Wire.read();
    }
    Serial.println(data);
    logSD(data);
    transmitXbee(data);
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
    int i = 0;
    while (str[i] != '\0'){
        logger.print(str[i], HEX);
    }
}

void transmitXbee(char* str){
    int i = 0;
    while (str[i] != '\0'){
        Serial.print(str[i], HEX);
    }
}

void serialDump(){
    while (Serial.available()){
        char d = Serial.read();
    }
}
