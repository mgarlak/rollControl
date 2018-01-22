
#include "rocketClass.hpp"
#include <MatrixMath.h>
#include <math.h>

rocket::rocket(){
	// Orientation Data
	pitch = 0;
	roll = 0;
	rollRate = 0;
	// Location Data and Trajectory
	// All values should be in ground frame.
	x = 0;   // Position x
	y = 0;   // Position y
	z = 0;   // Altitude
	xV = 0;  // Change in x
	yV = 0;  // Change in y
	zV = 0;  // Change in Altitude
	xA = 0;  
	yA = 0;  
	zA = 0;  
	rollUp2Date = false;
	pitchUp2Date = false;
	Adafruit_BMP280 bmp;
	Adafruit_BNO055 orient = Adafruit_BNO055(55);
}

double rocket::getSpeed(){
	return sqrt(getSpeedSq());
}
double rocket::getSpeedSq(){
	return xV*xV+yV*yV+zV*zV;
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

double rocket::getPitch(){
    /*if (!pitchUp2Date){
        float tempMatrix[9]={0};
        for(int i=0;i<9;++i) tempMatrix[i]=R[i]; //Need to copy the temp matrix
        Matrix.Multiply((float *)tempMatrix,(float *)up);
        
        pitch=acos(tempMatrix[0]*up[0]+tempMatrix[3]*up[1]+tempMatrix[6]+up[2]);
    }
    pitchUp2Date = true;*/
    return pitch;
}

double rocket::getRoll(){
    if(!rollUp2Date){
        //float tempMatrix[9]={0};
        float rocketNorth[3]={0};
        //for(int i=0;i<9;++i) tempMatrix[i]=R[i]; //Need to copy the temp matrix
        Matrix.Multiply((float *)R,(float *)north,3,3,1,(float*)rocketNorth);
        
        roll= atan(rocketNorth[0]/rocketNorth[1]);
    }
    rollUp2Date = true;
    return roll;
}

int rocket::updateRotMatrix(){
    double s=pow(Q[0]*Q[0]+Q[1]*Q[1]+Q[2]*Q[2]+Q[3]*Q[3],-2);
    
    R[0]=1-2*s*(Q[1]*Q[1]+Q[2]*Q[2]); R[1]=2*s*(Q[1]*Q[2]-Q[3]*Q[0]); R[2]=2*s*(Q[1]*Q[3]+Q[2]*Q[0]);
    R[3]=2*s*(Q[1]*Q[2]+Q[3]*Q[0]); R[4]=1-2*s*(Q[1]*Q[1]+Q[3]*Q[3]); R[5]=2*s*(Q[2]*Q[3]-Q[1]*Q[0]);
    R[6]=2*s*(Q[1]*Q[2]+Q[2]*Q[0]); R[7]=2*s*(Q[2]*Q[3]+Q[1]*Q[0]); R[8]=1-2*s*(Q[1]*Q[1]+Q[2]*Q[2]);

    rollMatrixUp2Date=true;

    return 0;
}

double rocket::getRollRate(){
	//Should be nearly identical to get roll, except using vQ instead of Q. 
}
