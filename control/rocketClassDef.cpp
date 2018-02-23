#include "rocketClass.hpp"

//Readability aid macros:
#define q_x Q[0]
#define q_y Q[1]
#define q_z Q[2]
#define q_w Q[3]
#define SQ(x) x*x
#define PI 2*asin(1)

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
        updateRotMatrix();
        float oldRoll=roll;

        float rocketNorth[3]={0};
        float rocketUp[3]={0};

        Matrix.Multiply((float *)R,(float *)up,3,3,1,(float*)rocketUp);
        /*
        take the cross product of rocketUp and up.  //The resulting vector points along the axis of rotation needed to digitally make the rocket point up.
        get the angle between rocketUp and up using a cross b = a*b*sin(\theta)
        normalize that vector.
        using the angle and the normalized cross product, construct a quaternion.
        using that quaternion, make a rotation matrix.
        multiply the two rotation matricies to produce a new one //This will only rotate rocket north about up.
        */

        Matrix.Multiply((float *)R,(float *)north,3,3,1,(float*)rocketNorth);//TODO: swap R with the matrix mentioned above
        Matrix.Print((float*)rocketNorth,3,1,"n");

        if (rocketNorth[1]==0){
            roll = rocketNorth[0]>0 ? PI/2.0 : (3.0/2.0)*PI;
        } else roll= (rocketNorth[0]>0 ? 0 : PI) + atan(rocketNorth[0]/rocketNorth[1]);//TODO: make sure this accurately calculates roll for all angles
        rollRate=1000.0*(roll-oldRoll)/float(deltaT);
    }
    rollUp2Date = true;
    return roll;
}

int rocket::updateRotMatrix(){
    if(!rollMatrixUp2Date){
        float q =  (SQ(q_x) + SQ(q_y) + SQ(q_z) + SQ(q_w));//Magnatude of the quaternion squared;
        float s = (q == 0) ? 1 : (1.0 / q);
    
        R[0] = 1 - 2 * s*(SQ(q_y) + SQ(q_z)); R[1] = 2 * s*(q_x*q_y - q_z*q_w); R[2] = 2 * s*(q_x*q_z+q_y*q_w);
	    R[3] = 2 * s*(q_x*q_y+q_z*q_w); R[4] = 1 - 2 * s*(Q[0] * Q[0] + Q[2] * Q[2]); R[5] = 2 * s*(q_y*q_z-q_x*q_w);
	    R[6] = 2 * s*(q_x*q_z + q_y * q_w); R[7] = 2 * s*(q_y*q_z + q_x * q_w); R[8] = 1 - 2 * s*(SQ(q_x) + SQ(q_y));

        rollMatrixUp2Date=true;
    }    


    return 0;
}

float rocket::getRollRate(){
    getRoll();
	return rollRate;
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
