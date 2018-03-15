#include "rocketClass.hpp"

//Readability aid macros:
#define q_x Q[0]
#define q_y Q[1]
#define q_z Q[2]
#define q_w Q[3]
#define SQ(x) x*x


rocket::rocket(Adafruit_BNO055 &BNO, Adafruit_BMP280 &BMP){
    //sensors
    bno=BNO;
    baro=BMP;

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
    rollMatrixUp2Date = false;
    speedUp2Date = false;

    //Get the gravity vector and the magnetic field vector
    imu::Vector<3> g=bno.getVector(VECTOR_GRAVITY);
    imu::Vector<3> m=bno.getVector(VECTOR_MAGNETOMETER);

    //Compute the up vector.  -g/|g|
    g.normalize();
    g=g*(-1);

    //Compute the north vector.  the magnetic field - the projection of the magnetic field on the up vector.
    m.normalize();
    imu::Vector<3>n=m-(g*m.dot(g));

    


    /*
    
    Get the gravity vector;
    normalize it;
    multiply it by -1
    set up to that.
    
    get the magnetic field vector.
    take the projection of the magnetic field vector on the up vector
    subtract that projection from the magnetic vector
    that's your north vector (or maybe the north vector multiplied by -1)
    */


    omega = 0;
    moi = 0;
}

float rocket::getSpeed(){
	if(!speedUp2Date){
        float inverseR[9];
        Matrix.Copy((float *)R,3,3,(float *)inverseR);
        Matrix.Invert((float *)inverseR,3);
        /*
        rotate the acceleration vector by the inverse of the rotation matrix
        add the acceleration vector to the velocity vector.

        */
    }
    return /*the square root of the dot product of rocketUp and the velocity vector*/;
}
float rocket::getSpeedSq(){
	return SQ(getSpeed());
}

int rocket::updateSensorData(){
    long current=millis();
    if(current-lastUpdate>10){
        deltaT=current-lastUpdate;
        lastUpdate=current;

        imu::Quaternion quat = bno.getQuat();
        q_x = quat.x();
        q_y = quat.y();
        q_z = quat.z();
        q_w = quat.w();
        oldZ=z;
        z = baro.readAltitude(1013.25 /*HARDCODED, WE'LL CHANGE LATER.  ADD TO CONFIG*/);

        imu::vector<3> acell=bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
        float aTemp[3];
        aTemp[0]=acell.x();
        aTemp[1]=acell.y();
        aTemp[2]=acell.z();

        

        pitchUp2Date = false;
        rollUp2Date = false;
        rollMatrixUp2Date = false;
        speedUp2Date = false;

        return 1;
    }
    return 0;
}

float rocket::getPitch(){
    if (!pitchUp2Date){
        float rocketUp[3]={0};
        Matrix.Multiply((float *)R,(float *)up,3,3,1,(float*)rocketUp);

        pitch=asin(dotProd((float*)up,(float*)rocketUp));
    }
    pitchUp2Date = true;
    return pitch;
}

float rocket::getRoll(){
    if(!rollUp2Date){
        updateRotMatrix();
        float oldRoll=roll;

        float rocketNorth[3]={0};
        float rocketUp[3]={0};
        float axisOfRot[3]={0};

        Matrix.Multiply((float *)R,(float *)up,3,3,1,(float*)rocketUp);

        cross((float*)up,(float*)rocketUp,(float*)axisOfRot);//May need to change the order of up and rocket up?
        float angle2Vert=asin(vecMag((float*)axisOfRot,3));
        normalize((float*)axisOfRot,(float*)axisOfRot);

        //Calculate quaternion that will turn the rockets pointing vector straight up
        float Qw=cos(angle2Vert/2);
        float Qx=sin(angle2Vert/2)*axisOfRot[0];
        float Qy=sin(angle2Vert/2)*axisOfRot[1];
        float Qz=sin(angle2Vert/2)*axisOfRot[2];

        float R2[9]={0};
        float R3[9]={0};

        R2[0] = 1 - 2 * (SQ(Qy) + SQ(Qz)); R2[1] = 2 * (Qx*Qy - Qz*Qw); R2[2] = 2 * (Qx*Qz+Qy*Qw);
	      R2[3] = 2 * (Qx*Qy+Qz*Qw); R2[4] = 1 - 2 * (SQ(Qx) + SQ(q_z)); R2[5] = 2 * (Qy*Qz-Qx*Qw);
	      R2[6] = 2 * (Qx*Qz + Qy * Qw); R2[7] = 2 * (Qy*Qz + Qx * Qw); R[8] = 1 - 2 * (SQ(Qx) + SQ(Qy));

        Matrix.Multiply((float*)R,(float*)R2,3,3,3,(float*)R3);

        Matrix.Multiply((float *)R3,(float *)north,3,3,1,(float*)rocketNorth);//TODO: swap R with the matrix mentioned above
        Matrix.Print((float*)rocketNorth,3,1,"n"); //Remove after we've check that the vertical component is negligible.

        if (rocketNorth[1]==0){
            roll = rocketNorth[0]>0 ? PI/2.0 : (3.0/2.0)*PI;
        } else roll= (rocketNorth[0]>0 ? 0 : PI) + atan(rocketNorth[0]/rocketNorth[1]);//TODO: make sure this accurately calculates roll for all angles
        
        //Calculate roll rate:
        if(oldRoll > 3.0/2.0*PI && roll < 1.0/4.0*PI){
            rollRate=1000.0*(roll-oldRoll+2.0*PI)/float(deltaT);
        } else if(roll > 3.0/2.0*PI && oldRoll < 1.0/2.0*PI){
            rollRate=1000.0*(roll-oldRoll)/float(deltaT);
        } else rollRate=1000.0*(roll-oldRoll-2.0*PI)/float(deltaT);
    }
    rollUp2Date = true;
    return roll;
}

int rocket::updateRotMatrix(){
    if(!rollMatrixUp2Date){
        float q =  (SQ(q_x) + SQ(q_y) + SQ(q_z) + SQ(q_w));//Magnatude of the quaternion squared;
        float s = (q == 0) ? 1 : (1.0 / q);
    
        R[0] = 1 - 2 * s*(SQ(q_y) + SQ(q_z)); R[1] = 2 * s*(q_x*q_y - q_z*q_w); R[2] = 2 * s*(q_x*q_z+q_y*q_w);
	    R[3] = 2 * s*(q_x*q_y+q_z*q_w); R[4] = 1 - 2 * s*(SQ(q_x) + SQ(q_z)); R[5] = 2 * s*(q_y*q_z-q_x*q_w);
	    R[6] = 2 * s*(q_x*q_z + q_y * q_w); R[7] = 2 * s*(q_y*q_z + q_x * q_w); R[8] = 1 - 2 * s*(SQ(q_x) + SQ(q_y));
      
    }    

    rollMatrixUp2Date=true;
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
