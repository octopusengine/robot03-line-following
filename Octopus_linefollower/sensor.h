// sensor.h

#ifndef _SENSOR_h
#define _SENSOR_h

#include "definitions.h"


class LineSensor
{
private:
	static const int _count = 4;
	int _min[_count] = {5000, 5000, 5000, 5000};
	int _max[_count] = {0, 0, 0, 0 };
	int _values[_count];
	double _error;

	

public:
	LineSensor();

	void readRaw(int &LL, int &L, int &P, int &PP);
	void readRaw();
	void kalibrate();
	void printValues();

	double getError();

};




#endif

