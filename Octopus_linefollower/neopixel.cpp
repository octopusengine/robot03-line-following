// 
// 
// 

#include "neopixel.h"


Neopixel::Neopixel() {
		FastLED.addLeds<NEOPIXEL, _PIN_NEOPIXEL>(_pixel, 1);
		_updatePixel(CRGB::Black);
	}

	void Neopixel::_updatePixel(CRGB _color) {
		static CRGB col = 0;

		if (_color != col) {
			col = _color;
			_pixel[0] = _color;
			FastLED.show();
		}
	}

	void Neopixel::forceColor(CRGB rgb) {
		_pixel[0] = rgb;
		FastLED.show();
	}

	void Neopixel::setColor(CRGB rgb) {
		_color = rgb;
	}

	void Neopixel::setBlink(bool blink) {
		_blinking = blink;
		_blink_delay = ALARM_BLINK_DELAY;
	}

	void Neopixel::setBlink(bool blink, int dylej) {
		_blinking = blink;
		_blink_delay = dylej;
	}

	void Neopixel::loop() {
		static unsigned long last_millis = 0;
		static bool blink = LOW;

		if (_blinking) {
			if (millis() - last_millis > _blink_delay) {
				last_millis = millis();

				blink = !blink;

				if (blink) {
					_updatePixel(CRGB::Black);
				}
				else {
					_updatePixel(_color);
				}
			}
		}
		else {
			_updatePixel(_color);
		}
	}


