#include "rocketClass.hpp"

//Readability aid macros:
#define q_x Q[0]
#define q_y Q[1]
#define q_z Q[2]
#define q_w Q[3]
#define SQ(x) x*x

rocket::rocket(){
	  // Orientation Data
	  pitch = 0;
	  roll = 0;
	  rollRate = 0;
	  // Location Data and Trajectory
	  // All values should be in ground frame.
	  z = 0;   // Altitude
	  zV = 0;  // Change in Altitude
	  rollUp2Date = false;
	  pitchUp2Date = false;
	  Adafruit_BMP280 bmp;
	  Adafruit_BNO055 orient = Adafruit_BNO055(55);
    omega = 0;
    moi = 0;
}

float rocket::getSpeed(){
	return sqrt(getSpeedSq());
}
float rocket::getSpeedSq(){
	return 0;
}

int rocket::updateSensorData(Adafruit_BNO055 &bno, Adafruit_BMP280 &baro){
    imu::Quaternion quat = bno.getQuat();
    q_x = quat.x();
    q_y = quat.y();
    q_z = quat.z();
    q_w = quat.w();
    z = baro.readAltitude(1013.25 /*HARDCODED, WE'LL CHANGE LATER*/);
    pitchUp2Date = false;
    rollUp2Date = false;
    return 0;
}

float rocket::getPitch(){
    /*if (!pitchUp2Date){
        float tempMatrix[9]={0};
        for(int i=0;i<9;++i) tempMatrix[i]=R[i]; //Need to copy the temp matrix
        Matrix.Multiply((float *)tempMatrix,(float *)up);
        
        pitch=acos(tempMatrix[0]*up[0]+tempMatrix[3]*up[1]+tempMatrix[6]+up[2]);
    }
    pitchUp2Date = true;*/
    return pitch;
}

float rocket::getRoll(){
    if(!rollUp2Date){
        //float tempMatrix[9]={0};
        float rocketNorth[3]={0};
        Matrix.Print((float*)rocketNorth,3,1,"n");
        //for(int i=0;i<9;++i) tempMatrix[i]=R[i]; //Need to copy the temp matrix
        Matrix.Multiply((float *)R,(float *)north,3,3,1,(float*)rocketNorth);
        Matrix.Print((float*)rocketNorth,3,1,"n");
        
        roll= atan(rocketNorth[0]/rocketNorth[1]);
    }
    rollUp2Date = true;
    return roll;
}

int rocket::updateRotMatrix(){    
    float q =  (SQ(q_x) + SQ(q_y) + SQ(q_z) + SQ(q_w));//Magnatude of the quaternion squared;
    float s = (q == 0) ? 1 : (1.0 / q);
    
    R[0] = 1 - 2 * s*(SQ(q_y) + SQ(q_z)); R[1] = 2 * s*(q_x*q_y - q_z*q_w); R[2] = 2 * s*(q_x*q_z+q_y*q_w);
	R[3] = 2 * s*(q_x*q_y+q_z*q_w); R[4] = 1 - 2 * s*(Q[0] * Q[0] + Q[2] * Q[2]); R[5] = 2 * s*(q_y*q_z-q_x*q_w);
	R[6] = 2 * s*(q_x*q_z + q_y * q_w); R[7] = 2 * s*(q_y*q_z + q_x * q_w); R[8] = 1 - 2 * s*(SQ(q_x) + SQ(q_y));

    rollMatrixUp2Date=true;

    return 0;
}

float rocket::getRollRate(){
	//Should be nearly identical to get roll, except using vQ instead of Q. 
}




int rocket::fillModel(int fpsize, int devName){
    int property = 0;
    while (property < numOfCParams){
        char* str = nullptr;
        Wire.requestFrom(commsDevice, numBytes);
        while (Wire.available()){
            char ch = Wire.read();
            if (ch == -1) break;
            str = caAppend(str, ch);
        }
        switch (property){
            case 0: omega = catof(str); break;
            case 1: moi = catof(str); break;
            case 2: plan.parseFlightPlan(str); break;
        }
        {
          delete[] str;
          str = nullptr;
        }
        ++property;
    }
    return 0;
}

int rocket::logData(char* fname, int floatSize){
    
}
