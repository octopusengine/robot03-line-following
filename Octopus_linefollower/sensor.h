// sensor.h

#ifndef _SENSOR_h
#define _SENSOR_h

#include <Arduino.h>

class Sensor
{
private:
	int _pin;

public:
	Sensor();
	Sensor(int pin);
	int read();

};




#endif

