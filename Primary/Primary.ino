/*
	This is for the primary module of Project P.A.S.T.
*/

/*
	Inputs:
		1. Gyroscope data
		2. Real time clock (RTC) data

	Outputs:
		1. CW - base motor clockwise rotation
		2. CCW - base motor counter clockwise rotation
		3. stah - sbase motor stop
		4. cw2 - dec motor clockwise rotation
		5. ccw2 - dec motor counter clockwise rotation
		6. stahp2 - dec motor stop
*/

// #include <virtualbotixRTC.h>
#include <Arduino.h>
#include <Wire.h>
#include <MPU9250.h>
#include <uRTCLib.h>

MPU9250 primaryMPU;

char dow[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

// Defining the RTC
uRTCLib primaryRTC(0x68); // 0x68 is the default I2C addres for the RTC module

double M,Y,D,MN,H,S; // Month, Year, Day, Minute, Hour, Second
double A,B;
double location = -115.287539; // My current longitude - Las Vegas
double lstDegrees; // Local sidereal time (lst) ind egrees
double lstHours; // Local side real time in decimal hours

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
	Serial.begin(115200);

	// Set date-time accordin to (seconds, minutes, hours, day of the week)
	primaryRTC.set(0, 05, 21, 2, 22, 7, 2025);  

	primaryMPU.setup(0x68);

	// Start communication with IMU
	//int status = primaryMPU.begin();

	if(!primaryMPU.setup(0x68))
	{
		Serial.println("IMU initialization unsuccessful");
		Serial.println("Status: ");

		while(1){}
	}  

  pinMode(stahp,OUTPUT);  
  pinMode(cw,OUTPUT);  
  pinMode(ccw,OUTPUT);  
  pinMode(stahp2,OUTPUT);  
  pinMode(cw2,OUTPUT);  
	pinMode(ccw2,OUTPUT);

	delay(5000);//wait before starting  	  
    	
	primaryMPU.calibrateAccelGyro();  
  primaryMPU.calibrateMag(); 

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
	receiveData();
	pitchCheck();
	yawCheck();
	timer = millis();
	// Vector norm = primaryMPU.readNormalizeGyro();

	// yaw = yaw + norm.YAxis * timeStep;
	// pitch = pitch + norm.XAxis * timeStep;

	if (primaryMPU.update()) 
	{
    float gyroX = primaryMPU.getGyroX();
    float gyroY = primaryMPU.getGyroY();
    float gyroZ = primaryMPU.getGyroZ();
    
    yaw = yaw + gyroY * timeStep;
    pitch = pitch + gyroX * timeStep;
	}

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
	if(Serial.available() > 0)
	{
		String a = Serial.readString();
		String v1, v2;

		// Separate string into parts and assign them to variables
		for(int i = 0; i < a.length(); i++)
		{
			if(a.substring(i, i+1) == ",")
			{
				v2 = a.substring(0, i);
				v1 = a.substring(i+1);
				
				break;
			}
		}

		val = 90 - v1.toFloat();
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
	
	if((floor(yaw * 100)) == (floor(val2 * 100)))
	{  
  	digitalWrite(stahp2,HIGH);  
  }
	else
	{  
  	digitalWrite(stahp2,LOW);  
  }  
  
	if((floor(yaw * 100)) < (floor(val2 * 100)))
	{  
		digitalWrite(cw2,HIGH);  
  }
	else
	{  
  	digitalWrite(cw2,LOW);  
  }  
  
	if((floor(yaw * 100)) > (floor(val2 * 100)))
	{  
  	digitalWrite(ccw2,HIGH);  
  }
	else
	{  
  	digitalWrite(ccw2,LOW);  
  }  

}

/* 
	This function calculates the local sidereal time 

	Parameters: none
	returns: none
*/
void lstTime()
{
	//Calculates local sidereal time based on this calculation,  

	M = (double) primaryRTC.month();  
	Y = (double) primaryRTC.year();  
	D = (double) primaryRTC.day();  
	MN = (double) primaryRTC.minute();  
	H = (double) primaryRTC.hour();  
	S = (double) primaryRTC.second();  
	A = (double)(Y-2000) * 365.242199;  
	B = (double)(M-1) * 30.4368499; 

	double JDN2000 = A+B+(D-1) + (primaryRTC.hour() / 24);  
	double decimalTime = H + (MN / 60)+(S / 3600);  
  double LST = 100.46 + 0.985647 * JDN2000 + location + 15 * decimalTime;  

  lstDegrees = (LST- (floor(LST / 360) * 360));  
  lstHours = lstDegrees / 15; 

}

