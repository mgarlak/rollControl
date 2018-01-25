#include "flightplan.hpp"
#include <SPI.h>
#include <SD.h>

struct rParams {
	double omega;
	double moi;
	/*insert new things as needed*/
	flightplan plan;
};

int parseConfig(File file, rParams &params, int paramNum){
	file = SD.open("rocket.config");
	if (file){
		int property = 0;
		while (file.available()){
			char ch = file.read();
			if (ch == '\n' && property == (paramNum-1)){ /*Then we know we have a full number value*/
				double val;
				/*convert char array/string to double*/
				switch (property){	/*Add new cases depending on how many properties are in the config file*/
					case 0:	params.omega = val; break;
					case 1:	params.moi = val; break;
				}
			else if (ch == '\n'){  /*Iterated over all the properties, now get flight plan*/
				/*get flight plan*/
			}
			++property;
			}
		}
	}
}

