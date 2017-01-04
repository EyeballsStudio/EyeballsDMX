/*---------------------------------------------
Eyeballs DMX library
---------------------------------------------*/

#ifndef EYEBALLS_DMX_H
#define EYEBALLS_DMX_H

//Ports used for reading the address through a DIP-switch

#define PORT_CLOCK 2
#define PORT_DATA 7
#define PORT_LOAD 4

#include <Arduino.h>
#include <Servo.h>
#include "Conceptinetics.h"

class EyeballsDMX {
private:
	int _ports[13];
	int _channels[13];
	int _bitMask[8];
	int _address;
	int _range;
	bool _arrayMapped = false;
	int _arrayMap[8];
	int _servoMap[12];
	int _arrayChannel;
	DMX_Slave _slave;
	Servo _servo[12];
public:
	EyeballsDMX();
	EyeballsDMX(int address, int range);
	void begin();
	word getAddress();
	void mapPwm(int pin, int channel);
	void mapDigital(int pin, int channel);
	void mapDigitalArray (int pins[], int channel);
	void mapServo(int index, int pin, int channel);
	void execute();
	void dummy();
};

#endif
