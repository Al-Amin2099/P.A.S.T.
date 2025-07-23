/*
	This is for the primary module of Project P.A.S.T.
	
*/

// #include <virtualbotixRTC.h>
#include <Arduino.h>
#include <Wire.h>
#include <MPU6050.h>
#include <uRTCLib>

MPU6050 primaryMPU(Wire, 0x68);

char dow[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

// Defining the RTC
uRTCLib primaryRTC(0x68); // 0x68 is the default I2C addres for the RTC module

double M,Y,D,MN,H,S;
double A,B;
double location = -117.9175657;
double lstDegrees;
double lstHours;

unsigned long timer = 0;

float timeStep = 0.01;

// Pitch and Yaw values
double pitch = 0;
double yaw = 0;
double val = 0; // Variable to store user input DEC
double val2 = 0; // Variable to store user input RA
double temp = val2; // Temp value to store val2

const int stahp = 7, stahp2 = 10;
const int cw = 8, cw2 = 11;
const int ccw = 6, ccw2 = 9;


void setup() 
{
	// Set date-time accordin to (seconds, minutes, hours, day of the week)
	primaryRTC.set(0, 05, 21, 2, 22, 7, 2025);  
    	
	// Start communication with IMU
	int status = primaryMPU.begin();

	if(status < 0)
	{
		Serial.println("IMU initialization unsuccessful");
		Serial.println("Status: ");
		Serial.println(status);

		while(1){}
	}

	Serial.begin(115200);  
   	pinMode(stahp,OUTPUT);  
    	pinMode(cw,OUTPUT);  
    	pinMode(ccw,OUTPUT);  
    	pinMode(stahp2,OUTPUT);  
    	pinMode(cw2,OUTPUT);  
    	pinMode(ccw2,OUTPUT);

    	
	delay(5000);//wait before starting  	  
    	
	mpu.calibrateGyro();  
    	mpu.setThreshold(3); 

}

void loop()
{
	primaryRTC.refresh();

	if(floor(lstDegrees) == lstDegrees)
	{
		if(lstDegrees > 100)
		{
			val2 = temp + (360 - lstDegrees);
		}
		else
		{
			val2 = temp - lstDegrees;
		}
	}

	//primaryRTC.updateTime();
	lstTime();
	recvdata();
	pitchCheck();
	yawCheck();
	timer = millis();
	Vector norm = mpu.readNormalizeGyro();

	yaw = yaw + norm.YAxis * timeStep;
	pitch = pitch + norm.XAxis * timeStep;

	Serial.print("Yaw = ");
	Serial.println(yaw);

	Serial.print("Pitch = ");
	Serial.println(pitch);
	
	Serial.print("lstDegrees: ");
	Serial.println(lstDegrees);

	Serial.print("lstHours: ");
	Serial.println(lstHours); // Local sidereal time in decimal hours

	delay((timeStep * 1000) - (millis() - timer)); // Timer for the gyro
}


/*
	This function receives data from serial as (0.00, 0.00),
	Splits strings by the comma, then converts to doubles.

	Paramerters: none
	Returns: none

*/
void receiveData()
{
	if(Serial.available() > -)
	{
		String a = Serial.readString();
		String v1, v2;

		
		// Separate string into parts and assign them to variables
		for(int i = 0; i < a.length(); i++)
		{
			if(a.substring(i, i+1) == ",")
			{
				val2 = a.substring(0, i);
				val1 = a.substring(i=1);
				
				break;
			}
		}

		val = 90 = v1.toFloat();
		val2 = v2.toFloat();
		temp = val2;
	}
}

/*
	This function checks the pitch value

	Parameters: none
	Returns: none

*/
void pitchCheck()
{
	if((floor(pitch * 100)/100) == (floor(val * 100)/100))
	{
		digitalWrite(stahp, HIGH);
	}
	else
	{
		digitalWrite(stahp, LOW);
	}

	if((floor(pitch * 100)) < (floor(val * 100)))
	{
		digitalWrite(cw, HIGH);
	}
	else
	{
		digitalWrite(cw, LOW);
	}

	if((floor(pitch * 100)) > (floor(val * 100)))
	{
		digitalWrite(ccw, HIGH);
	}
	else
	{	
		digitalWrite(ccw, LOW);
	}
}

/*
	This function checks the yaw value
	
	Parameters: none
	Returns: none
*/
void yawCheck()
{
	
}

/* 
	This function calculates the local sidereal time 

	Parameters: none
	returns: none
*/
void lstTime()
{
	
}

