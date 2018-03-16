#include "rocketClass.hpp"

//Readability aid macros:
#define q_x Q[0]
#define q_y Q[1]
#define q_z Q[2]
#define q_w Q[3]
#define SQ(x) x*x
#define packetSize 42

rocket::rocket(Adafruit_BNO055 *BNO, Adafruit_BMP280 *BMP){
    //sensors
    bno=BNO;
    baro=BMP;

    // Orientation Data
    pitch = 0;
    roll = 0;
    rollRate = 0;
    // Location Data and Trajectory
    // All values should be in ground frame.
    rollUp2Date = false;
    pitchUp2Date = false;
    rollMatrixUp2Date = false;
    speedUp2Date = false;

    //Get the gravity vector and the magnetic field vector
    imu::Vector<3> g=bno->getVector(Adafruit_BNO055::VECTOR_GRAVITY);
    imu::Vector<3> m=bno->getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);

    //Compute the up vector.  -g/|g|
    g.normalize();
    g=g*(-1);

    up[0]=g.x();
    up[1]=g.y();
    up[2]=g.z();

    //Compute the north vector.  the magnetic field - the projection of the magnetic field on the up vector.
    m.normalize();
    imu::Vector<3>n=m-(g*m.dot(g));

    north[0]=n.x();
    north[1]=n.y();
    north[2]=n.z();

    omega = 0;
    moi = 0;
}

float rocket::getSpeed(){
    return sqrt(getSpeedSq());
}
float rocket::getSpeedSq(){
	if(!speedUp2Date){
        float inverseR[9];
        Matrix.Copy((float *)R,3,3,(float *)inverseR);
        Matrix.Invert((float *)inverseR,3);
        Matrix.Multiply((float *)inverseR,(float *)aNRocketFrame,3,3,1,(float * )a);

        v[0]+=(1000000.0/((float)deltaT))*a[0];
        v[1]+=(1000000.0/((float)deltaT))*a[1];
        v[2]+=(1000000.0/((float)deltaT))*a[2];
    }
    updateRotMatrix();
    float rocketUp[3]={0};
    Matrix.Multiply((float *)R,(float *)up,3,3,1,(float*)rocketUp);

    return dotProd((float *)v,(float *) rocketUp);
}

int rocket::updateSensorData(){
    long current=millis();
    if(current-lastUpdate>10){
        deltaT=current-lastUpdate;
        lastUpdate=current;

        imu::Quaternion quat = bno->getQuat();
        q_x = quat.x();
        q_y = quat.y();
        q_z = quat.z();
        q_w = quat.w();

        T=baro->readTemperature();
        P=baro->readPressure();


        imu::Vector<3> acell = bno->getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
        aNRocketFrame[0]=acell.x();
        aNRocketFrame[1]=acell.y();
        aNRocketFrame[2]=acell.z();

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
        updateRotMatrix();
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
        float east[3]={0};
        float rocketUp[3]={0};

        float R3[9]={0};


        Matrix.Multiply((float *)R,(float *)up,3,3,1,(float*)rocketUp);

        //calculate the east unit vector
        cross((float *)north,(float *)up,(float *)east);
        normalize((float *)east,(float *)east);
        if(dotProd((float*)up,(float *)rocketUp)<0.9999){ // Breaks for the rocket pointed stragith down.  Hopefully won't be an issue.
            float axisOfRot[3] = {0};
            
            cross((float*)up,(float*)rocketUp,(float*)axisOfRot);//May need to change the order of up and rocket up?
            float angle2Vert=asin(vecMag((float*)axisOfRot,3));
            normalize((float*)axisOfRot,(float*)axisOfRot);

            //Calculate quaternion that will turn the rockets pointing vector straight up
            float Qw=cos(angle2Vert/2);
            float Qx=sin(angle2Vert/2)*axisOfRot[0];
            float Qy=sin(angle2Vert/2)*axisOfRot[1];
            float Qz=sin(angle2Vert/2)*axisOfRot[2];

            //Calculate the rotation matric.
            float R2[9]={0};

            R2[0] = 1 - 2 * (SQ(Qy) + SQ(Qz)); R2[1] = 2 * (Qx*Qy - Qz*Qw); R2[2] = 2 * (Qx*Qz+Qy*Qw);
	        R2[3] = 2 * (Qx*Qy+Qz*Qw); R2[4] = 1 - 2 * (SQ(Qx) + SQ(q_z)); R2[5] = 2 * (Qy*Qz-Qx*Qw);
	        R2[6] = 2 * (Qx*Qz + Qy * Qw); R2[7] = 2 * (Qy*Qz + Qx * Qw); R[8] = 1 - 2 * (SQ(Qx) + SQ(Qy));

            Matrix.Multiply((float*)R,(float*)R2,3,3,3,(float*)R3);

        } else {
            Matrix.Copy((float *)R,3,3,(float *)R3); // If pitch is near verticle, we don't need to do the "virtual pitch up"
        }

        Matrix.Multiply((float *)R3,(float *)north,3,3,1,(float*)rocketNorth);

        //Calculate the roll:
        if (dotProd((float *)rocketNorth,(float *)east)>0){
            roll=acos(dotProd((float *)rocketNorth,(float *)north));
        } else {
            roll=2.0*PI-acos(dotProd((float *)rocketNorth,(float *)north));
        }

        //Calculate roll rate:
        if(oldRoll > 7.0/4.0*PI && roll < 1.0/4.0*PI){ //Roll has likely passed from near all the way around the way around the circle through zero.
            rollRate=1000000.0*(roll-oldRoll+2.0*PI)/float(deltaT);
        } else if(roll > 7.0/4.0*PI && oldRoll < 1.0/4.0*PI){ //Roll has likely passed from barely around the circle through zero
            rollRate=1000000.0*(roll-oldRoll)/float(deltaT);
        } else rollRate=1000000.0*(roll-oldRoll-2.0*PI)/float(deltaT); //Roll has not passed through zero.
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
    // quaternion (16 bytes)
    for (; i < 4; ++i){
        toChar(Q[i], msg+(i*4));
    }
    // micros (timestamp) (4 bytes)
    unsigned long micro = micros();
    toChar(micro, msg+(i*4));
    ++i;
    // Pressure (4) bytes
    toChar(P, msg+(i*4));
    ++i;
    // Vector a(12 bytes)
    unsigned char it = 0;
    while (it < 3){
        toChar(aNRocketFrame[it], msg+(i*4));
        ++it; ++i;
    }

    // temp (4 bytes)
    toChar(T, msg+(i*4));
    ++i;
    // flight event (1 byte)
    msg[++i] = '1';

    //Serial.println("SENDING");
    Wire.beginTransmission(device);
    unsigned char* out = new unsigned char[(packetSize*2) + 1];
    toHex(msg, out, packetSize);
    char j = 0;
    while (j < packetSize){
        //Serial.print(out[j*2]);
        //Serial.print(out[(j*2)+1]);
        Wire.write(msg[j]);
        ++j;
    }

    Wire.endTransmission();
    delete[] out;
    out = nullptr;
    delete[] msg;
    msg = nullptr;
}
