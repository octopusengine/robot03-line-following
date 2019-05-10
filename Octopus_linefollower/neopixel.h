#ifndef NEOPIXEL_H
#define NEOPIXEL_H

#include <Adafruit_NeoPixel.h>
#include "octopuslibrary.h"


class Neopixel {

private:
	Adafruit_NeoPixel pixels;


public:
	Neopixel();
	void setColor(byte r, byte g, byte b);

};

#endif