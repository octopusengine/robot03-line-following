

#include "sensor.h"
#include "neopixel.h"
#include "button.h"
#include "motor.h"

#include "definitions.h"

#define CYCLES_PER_SECOND 200

int speed_forward = 90;
int speed_pidcorrection = 90;

//more in pid file
float kK = 1; //1 - constant of error "quadratic" bending
float pK = 0.75; //0.75 - p constant of pid regulator
float dK = 100; //100-120 - d constant of pid regualtor
float iK = 0; //0 - i constant of pid regulator - in our case non-usable
int n_EMA = 10; //5-10 - the length of sorrection smoothing exponencial moving average

boolean bot;

Motor motor;
Neopixel neopixel;
LineSensor sensor;
Button button(PIN_BUTTON);

void setup() {
	// the minimal speed depends on CYCLES_PER_SECOND defined
	Serial.begin(SERIAL_BAUD);

	motor.setLeft(0);
	motor.setRight(0);
}



void loop() {

	//you need to press the button for the program to start
	neopixel.forceColor(CRGB::Red);
	while(!button.read()) delay(1);
	neopixel.forceColor(CRGB::Green);
	while (button.read()) delay(1);
	delay(250);

	while (!button.read()) {

		int speed = 80;

		sensor.readRaw();
		sensor.printValues();
		
		double odchylka = sensor.getError();
	
		motor.setLeft(speed + odchylka * speed);
		motor.setRight(speed - odchylka * speed);
	}

	//if the button is pressed stop the motors
	motor.setLeft(0);
	motor.setRight(0);

	//wait for button unpress
	while (button.read()) delay(1);

}