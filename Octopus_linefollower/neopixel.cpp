// 
// 
// 

#include "neopixel.h"


Neopixel::Neopixel() : pixels(1, PIN_WS, NEO_GRB + NEO_KHZ800) {
	pixels.begin();
}

void Neopixel::setColor(byte r, byte g, byte b) {
	pixels.setPixelColor(0, pixels.Color(r/2, g/2, b/2));
	pixels.show();
}



