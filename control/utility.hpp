#ifndef UTILLITY_HPP
#define UTILLITY_HPP

#include"math.h"

union charFloatConverter{
    unsigned char b[4];
    float f;
};


//Float-byte converters for coms-control communication
float toFloat(unsigned char *);
void toChar(float, unsigned char *); 

//FlightPlan utility
bool isDigit(char);
bool areDigits(char*, int);
int appendCharDigit(int, char);
int getNumberFromChars(char*, int);

bool isFpVital(char);
int getCaSize(char*);
char* caAppend(char*, char);
int pftoi(float&);

void toHex(unsigned char*, char*, char);

//RocketClass utility

float catof(char*);

#endif
