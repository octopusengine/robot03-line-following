// 
// 
// 

#include "sensor.h"

Sensor::Sensor() {

	return;
	
}

Sensor::Sensor(int pin) {
	_pin = pin;
	pinMode(_pin, INPUT_PULLUP);
}

int Sensor::read() {
	return analogRead(_pin);
}