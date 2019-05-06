// 
// 
// 

#include "sensor.h"

LineSensor::LineSensor() {
	pinMode(PIN_LL, INPUT);
	pinMode(PIN_L, INPUT);
	pinMode(PIN_P, INPUT);
	pinMode(PIN_PP, INPUT);
}

void LineSensor::readRaw(int &LL, int &L, int &P, int &PP) {
	LL = _values[0] = analogRead(PIN_LL);
	L = _values[1] = analogRead(PIN_L);
	P = _values[2] = analogRead(PIN_P);
	PP = _values[3] = analogRead(PIN_PP);
}

void LineSensor::readRaw() {
	_values[0] = analogRead(PIN_LL);
	_values[1] = analogRead(PIN_L);
	_values[2] = analogRead(PIN_P);
	_values[3] = analogRead(PIN_PP);
}

void LineSensor::kalibrate() {

	//int readings[4];

	//for (int i = 0; i < _count; i++)
	//{
	//	_min[i] = 1023;
	//	_max[i] = 0;
	//}

	//while ((digitalRead(PIN_BUTTON) == LOW)) {
	//	delay(10);
	//}
	//delay(50);
	//while ((digitalRead(PIN_BUTTON) == HIGH)) {
	//	readRaw(readings[0], readings[1], readings[2], readings[3]);
	//	if (TESTMODE) {
	//		Vypi_C5_A1_plot();
	//	}
	//	Set_min_max();
	//	if (MinMaxChanged) {
	//	}
	//	delayMicroseconds(1000);
	//}
	//delay(200);
	//while ((digitalRead(BUTTON_PIN) == LOW)) {
	//	delay(10);
	//}
	//delay(50);
}


void LineSensor::printValues() {
	Serial.print(_values[0]);
	Serial.print(", ");
	Serial.print(_values[1]);
	Serial.print(", ");
	Serial.print(_values[2]);
	Serial.print(", ");
	Serial.println(_values[3]);
}


 
double LineSensor::getError() {
	const int hranice = 2500;

	if (_values[0] > hranice) {
		_error = -1;
	}

	if (_values[1] > hranice) {
		_error = -0.5;
	}

	if (_values[2] > hranice) {
		_error = 0.5;
	}

	if (_values[3] > hranice) {
		_error = 1;
	}

	return _error;
}
