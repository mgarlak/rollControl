#include "utility.hpp"
#include <SPI.h>
#include <SD.h>
#include <Wire.h>

#define configFile "ROCKETCF.TXT"
#define flightLog "flogger1.txt"
#define sdPin 10
#define fpacc 5
#define controlDevice 75
#define packetSize 22

File configf;
File logger;
int cmdSqnc = 0;
unsigned char* data = new unsigned char[packetSize];
char* fp2 = nullptr;
bool keepListening = false;

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
    Serial.println("LOOPING");
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
    delay(1000);
}

void requestHandler(){
    Serial.println(F("IN REQUEST"));
    switch (cmdSqnc){
        case 0: ackSD(); break;
        case 1: sendParam(); break;
        default: {}
    }
}

void receiveHandler(int bytesReceived){
    //int i = 0;
    Serial.println(F("IN RECEIVE"));
    char i = 0;
    while(Wire.available()){
        data[i] = Wire.read();      
        ++i;
    }
    i = 0;
    while (i < packetSize){
        Serial.print(data[i]);
        ++i;
    }
    Serial.println("");
    char* out = new char[(packetSize*2)+1];
    toHex(data, out, packetSize);
    char j = 0;
    while (j < packetSize){
        Serial.print(out[j*2]);
        Serial.print(out[j*2]+1);
        ++j;
    }
    Serial.println("");
    delete[] out;
    out = nullptr;
    /*for (int i = 0; Wire.available() && i < packetSize; ++i){
        data[i] = Wire.read();
        if (i%2 == 0){
            Serial.print("BAD");
        }
        else Serial.print("BAD");
        ++i;
    }
    for (int i = 0; i < packetSize; ++i){
      Serial.print(data[i]);
    }
    logSD(data);
    transmitXbee(data);*/
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
    while (i < packetSize){
        logger.print(str[i], HEX);
    }
    logger.close();
}

void transmitXbee(char* str){
    int i = 0;
    while (i < packetSize){
        Serial.print(str[i], HEX);
        ++i;
    }
}

void serialDump(){
    while (Serial.available()){
        char d = Serial.read();
    }
}
