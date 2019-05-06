// 
// 
// 

#include "button.h"



Button::Button(uint8_t pin) {
	_pin = pin;
	pinMode(_pin, INPUT_PULLUP);
}


bool Button::read() {
	if (!digitalRead(_pin)) {
		delay(10);
		return !digitalRead(_pin);
	}

	return false;
}