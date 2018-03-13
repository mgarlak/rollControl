#include "rocketClass.hpp"

//Readability aid macros:
#define q_x Q[0]
#define q_y Q[1]
#define q_z Q[2]
#define q_w Q[3]
#define SQ(x) x*x
#define packetSize 22

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
    rollMatrixUp2Date = false;
    speedUp2Date = false;
    calibrationPressure = 0;
    omega = 0;
    moi = 0;
}

float rocket::getSpeed(){
    if(!speedUp2Date){
        zV=(1000.0*(z-oldZ)/float(deltaT))/cos(getPitch());
        speedUp2Date=true;
    }
    return zV;
}
float rocket::getSpeedSq(){
    return SQ(getSpeed());
}

int rocket::updateSensorData(Adafruit_BNO055 &bno, Adafruit_BMP280 &baro){
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
        z = baro.readAltitude(calibrationPressure /*HARDCODED, WE'LL CHANGE LATER.  ADD TO CONFIG*/);

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
        /*
        multiply up by the rotation matrix to get the direction the rocket is pointing.
        take the dot product of up and rocketUp.  //Note: up and rocketUp must be unit vectors!
        calculate pitch as the inverse sine of that dot product
        */
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
        if(oldRoll > 3.0/4.0*PI && roll < 1.0/4.0*PI){
            rollRate=1000.0*(roll-oldRoll+2.0*PI)/float(deltaT);
        } else if(roll > 3.0/4.0*PI && oldRoll < 1.0/4.0*PI){
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
        R[3] = 2 * s*(q_x*q_y+q_z*q_w); R[4] = 1 - 2 * s*(Q[0] * Q[0] + Q[2] * Q[2]); R[5] = 2 * s*(q_y*q_z-q_x*q_w);
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
            case 2: calibrationPressure = catof(str); break;
            case 3: plan.parseFlightPlan(str); break;
        }
        {
            delete[] str;
            str = nullptr;
        }
        ++property;
    }
    return 0;
}

int rocket::sendDataComms(int device){
    unsigned char* msg = new unsigned char[packetSize];
    unsigned char i = 0;
    for (; i < 4; ++i){
        toChar(Q[i], msg+(i*4));
    }
    toChar(z, msg+(i*4));
    msg[++i] = '1';

    Serial.println("SEND DATA");
    int j = 0;
    Wire.beginTransmission(device);
    j = 0;
    char* out = new char[(packetSize*2) + 1];
    toHex(msg, out, packetSize);
    while (j < packetSize){
        Serial.print(out[j*2]);
        Serial.print(out[(j*2)+1]);
        Wire.write(msg[j]);
        ++j;
    }

    Wire.endTransmission();
    delete[] out;
    out = nullptr;
    delete[] msg;
    msg = nullptr;
}
