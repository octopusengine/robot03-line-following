#ifndef NEOPIXEL_H
#define NEOPIXEL_H

#include <FastLED.h>
#include "definitions.h"

#define ALARM_BLINK_DELAY 250
static const uint8_t _PIN_NEOPIXEL = PIN_NEOPIXEL;

class Neopixel {

private:
	CRGB _pixel[1];
	bool _blinking = false;
	int _blink_delay = ALARM_BLINK_DELAY;
	CRGB _color = CRGB::Black;
	void _updatePixel(CRGB _color);

public:
	Neopixel();
	void forceColor(CRGB rgb);
	void setColor(CRGB rgb);
	void setBlink(bool blink);

	void setBlink(bool blink, int dylej);
	void loop();
};

#endif