#include "rocketClass.hpp"

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
    Q[0] = quat.x();
    Q[1] = quat.y();
    Q[2] = quat.z();
    Q[3] = quat.w();
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
    float s=pow(Q[0]*Q[0]+Q[1]*Q[1]+Q[2]*Q[2]+Q[3]*Q[3],-2);
    
    R[0]=1-2*s*(Q[1]*Q[1]+Q[2]*Q[2]); R[1]=2*s*(Q[1]*Q[2]-Q[3]*Q[0]); R[2]=2*s*(Q[1]*Q[3]+Q[2]*Q[0]);
    R[3]=2*s*(Q[1]*Q[2]+Q[3]*Q[0]); R[4]=1-2*s*(Q[1]*Q[1]+Q[3]*Q[3]); R[5]=2*s*(Q[2]*Q[3]-Q[1]*Q[0]);
    R[6]=2*s*(Q[1]*Q[2]+Q[2]*Q[0]); R[7]=2*s*(Q[2]*Q[3]+Q[1]*Q[0]); R[8]=1-2*s*(Q[1]*Q[1]+Q[2]*Q[2]);

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
