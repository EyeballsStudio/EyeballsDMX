/*---------------------------------------------
Eyeballs DMX library
---------------------------------------------*/

#include "EyeballsDMX.h"

//Constructors

//----------
//EyeballsDMX() 
//Initializes for use with external address encoder

EyeballsDMX::EyeballsDMX() : _slave(512), _servo() {
	//Set pin modes
	pinMode(PORT_CLOCK, OUTPUT);
	pinMode(PORT_DATA, INPUT);
	pinMode(PORT_LOAD, OUTPUT);
	//Initialize the ports
	digitalWrite(PORT_LOAD, HIGH);
	digitalWrite(PORT_LOAD, HIGH);
	//Initializes arrays
	for (int c = 0; c < 13; c++) {
		_ports[c] = 0;
		_channels[c] = 0;
	}
	//Initialize bitmasks
	_bitMask[0] = 1;
	_bitMask[1] = 2;
	_bitMask[2] = 4;
	_bitMask[3] = 8;
	_bitMask[4] = 16;
	_bitMask[5] = 32;
	_bitMask[6] = 64;
	_bitMask[7] = 128;
}

//----------
//EyeballsDMX(int address, int range)
//Initializes with starting address and address range

EyeballsDMX::EyeballsDMX(int address, int range) : _slave(512), _servo() {
	//Store address and range
	_address = address;
	_range = range;
	//Initializes arrays
	for (int c = 0; c < 13; c++) {
		_ports[c] = 0;
		_channels[c] = 0;
	}
	//Initialize bitmasks
	_bitMask[0] = 1;
	_bitMask[1] = 2;
	_bitMask[2] = 4;
	_bitMask[3] = 8;
	_bitMask[4] = 16;
	_bitMask[5] = 32;
	_bitMask[6] = 64;
	_bitMask[7] = 128;
}

//Member functions

//----------
//begin function
//Initializes the DMX slave

void EyeballsDMX::begin() {
	//Enable DMX slave
	_slave.enable();
	_slave.setStartAddress(1);
}

//----------
//getAddress function
//Reads the address set on the DIP-switch

word EyeballsDMX::getAddress() {
	//Init data
	word data = 0;
	//Load the switches
	digitalWrite(PORT_LOAD, LOW);
	digitalWrite(PORT_LOAD, HIGH);
	//Read 9 bits of data
	for (int c = 0; c < 10; c++) {
		data = data << 1;
		data = data | word(digitalRead(PORT_DATA));
		digitalWrite(PORT_CLOCK, LOW);
		digitalWrite(PORT_CLOCK, HIGH);
	}
	//Return the result
	return data;
}

//----------
//mapPwm function
//Maps a DMX channel to a PWM Arduino pin

void EyeballsDMX::mapPwm(int pin, int channel) {
	//Sets array field to 1, means it's mapped as a PWM channel
	_ports[pin] = 1;
	//Stores the channel that's mapped to a pin
	_channels[pin] = channel;
}

//----------
//mapDigital function
//Maps a DMX channel to a digital Arduino pin (toggle treshold = 128)

void EyeballsDMX::mapDigital(int pin, int channel) {
	//Sets array field to 2, means it's mapped as a digital channel
	_ports[pin] = 2;
	//Stores the channel that's mapped to a pin
	_channels[pin] = channel;
}

//----------
//mapDigitalArray function
//Maps a single DMX channel to eight digital pins

void EyeballsDMX::mapDigitalArray(int pins[], int channel) {
	//Toggle the Mapped flag
	_arrayMapped = true;
	//Store the channel
	_arrayChannel = channel;
	//Store the pins
	for (int c = 0; c < 8; c++) {
		_arrayMap[c] = pins[c];
	}
}

//----------
//mapServo function
//Attaches a servo to a channel

void EyeballsDMX::mapServo(int index, int pin, int channel) {
	//Set array field to 3, indicating a mapped servo
	_ports[pin] = 3;
	//Store the channel and index
	_channels[pin] = channel;
	_servoMap[pin] = index;
	//Attach the servo
	_servo[index].attach(pin);
}

//----------
//execute function
//Read DMX channels and sends them to relevant outputs

void EyeballsDMX::execute() {
	int bitCounter = 0;
	int channelValue = 0;
	
	//Digital array mapped
	if (_arrayMapped) {
		//Get channel value
		channelValue = _slave.getChannelValue(_arrayChannel);
		//Cycle array map
		for (int c = 0; c < 8; c++) {
 			if ((channelValue & _bitMask[c]) == _bitMask[c]) digitalWrite(_arrayMap[c], HIGH);
			else digitalWrite(_arrayMap[c], LOW);
		}
	}
	//Cycle ports array
	for (int c = 0; c < 13; c++) {
		//Port is set to 1, meaning it's mapped as a PWM channel
		if (_ports[c] == 1) {
			//Get the channel value
			analogWrite(c,_slave.getChannelValue(_channels[c]));
		}
		//Port is set to 2, mapped as an on/off channel
		if (_ports[c] == 2) {
			if (_slave.getChannelValue(_channels[c]) > 127) digitalWrite(c,HIGH);
			else digitalWrite(c,LOW);
		}
		//Port set to 3, servo mapped
		if (_ports[c] == 3) {
			_servo[_servoMap[c]].write(round((float(_slave.getChannelValue(_channels[c]))/255)*180));
		}
	}
}

//-----------
//Dummy function

void EyeballsDMX::dummy() {
	analogWrite(3,_slave.getChannelValue(5));
}

