

#include "sensor.h"
#include "neopixel.h"
#include "button.h"
#include "motor.h"

#define WHITE 255,255,255
#define BLACK 0,0,0
#define BLUE 0,0,255
#define GREEN 0,255,0
#define ORANGE 255,128,0
#define RED 255,0,0
#define PURPLE 255,255,0


#define SERIAL_BAUD 115200



//modules
#define PIN_BUTTON PIN_SERVO3
#define PIN_NEOPIXEL PIN_WS

//analog sensors
#define PIN_LL PIN_I36
#define PIN_L PIN_I39
#define PIN_P PIN_I34
#define PIN_PP PIN_I35

#define SENSOR_COUNT 4

const int CYCLES_PER_SECOND = 500;

int speed_forward = 90;
int speed_correction = 90;

//more in pid file
float kK = 1; //1 - constant of error "quadratic" bending
float pK = 0.75; //0.75 - p constant of pid regulator
float dK = 100; //100-120 - d constant of pid regualtor
float iK = 0; //0 - i constant of pid regulator - in our case non-usable
int n_EMA = 10; //5-10 - the length of sorrection smoothing exponencial moving average

bool debug = false;

Motor motor;
Neopixel neopixel;
Sensor sensor[SENSOR_COUNT];
Button button(PIN_BUTTON);

int reading[SENSOR_COUNT];
int reading_min[SENSOR_COUNT];
int reading_max[SENSOR_COUNT];

double error;

int speed_left;
int speed_right;

bool running;
bool outOfTrack;
bool lastTurnRight;

void setup() {
	// the minimal speed depends on CYCLES_PER_SECOND defined
	Serial.begin(SERIAL_BAUD);
	Serial.println("Start");

	analogReadResolution(10);

	sensor[0] = Sensor(PIN_LL);
	sensor[1] = Sensor(PIN_L);
	sensor[2] = Sensor(PIN_P);
	sensor[3] = Sensor(PIN_PP);

	motor.setLeft(0);
	motor.setRight(0);

	if (button.read())
	{
		debug = true;
		neopixel.setColor(WHITE);
		delay(100);
		neopixel.setColor(BLACK);
		delay(100);
		neopixel.setColor(WHITE);
		delay(100);
		neopixel.setColor(BLACK);
		delay(100);
	}

	while (button.read()) delay(1);

	Serial.println("Setup done");
}



void loop() {

	neopixel.setColor(BLUE);
	while (!button.read()) delay(1);
	while (button.read()) delay(1);
	calibrate();

	while (true) {

		neopixel.setColor(RED);
		while (!button.read()) delay(1);
		neopixel.setColor(GREEN);
		while (button.read()) delay(1);
		delay(250);

		running = true;

		while (!button.read() && running) {

			readTrack();
			apllyCorrection();
			calculateError();
			calculateSpeed();
			if (!debug) {
				setMotors();
			}
			else {
				printValues();
			}
		}

		//if the button is pressed stop the motors
		motor.setLeft(0);
		motor.setRight(0);

		//wait for button unpress
		while (button.read()) delay(1);
	}
}

void calibrate() {
	neopixel.setColor(ORANGE);

	for (int i = 0; i < SENSOR_COUNT; i++) {
		reading_min[i] = 1024;
		reading_max[i] = 0;
	}

	while (!button.read()) {
		readTrack();
		printRedings();
		for (int i = 0; i < SENSOR_COUNT; i++) {
			if (reading[i] > reading_max[i])
				reading_max[i] = reading[i];
			if (reading[i] < reading_min[i])
				reading_min[i] = reading[i];
		}
	}

	printRedingsMinMax();

	while (button.read()) delay(1);
}

void readTrack() {
	reading[0] = analogRead(PIN_LL);
	reading[1] = analogRead(PIN_L);
	reading[2] = analogRead(PIN_P);
	reading[3] = analogRead(PIN_PP);
}

void apllyCorrection() {

	for (int i = 0; i < SENSOR_COUNT; i++) {
		if (reading[i] > reading_max[i])
			reading[i] = reading_max[i];
		if (reading[i] < reading_min[i])
			reading[i] = reading_min[i];

		reading[i] = map(reading[i], reading_min[i], reading_max[i], 0, 900);
	}

}

void calculateError() {
	int soucet1 = reading[0] * 1 + reading[1] * 2 + reading[2] * 3 + reading[3] * 4;
	int soucet2 = reading[0] + reading[1] + reading[2] + reading[3];


	if (soucet1 < 0) {
		digitalWrite(BUILT_IN_LED, HIGH);
		Serial.println("!!! Zaporny soucet - asi pretekl");
		running = false;
		error = 0.0;
		return;
	}

	if (soucet2 == 0 ) {
		if (!outOfTrack) {
			neopixel.setColor(PURPLE);
			outOfTrack = true;
		}
		error = 0.0;
		return;
	}

	else {
		if (outOfTrack) {
			neopixel.setColor(GREEN);
			outOfTrack = false;
		}
	}

	error = (double)soucet1 / (double)soucet2;
	error = error - 2.5;
	error = error / 1.5;

	if (error > 0.0) {
		lastTurnRight = true;
	}
	else {
		lastTurnRight = false;
	}
}

void calculateSpeed() {
	speed_left = speed_forward + error * speed_correction;
	speed_right = speed_forward - error * speed_correction;
}

void setMotors() {
	motor.setLeft(speed_left);
	motor.setRight(speed_right);
}

void printRedingsMinMax() {
	Serial.print("MINs: " + String(reading_min[0]));
	Serial.print(", " + String(reading_min[1]));
	Serial.print(", " + String(reading_min[2]));
	Serial.println(", " + String(reading_min[3]));

	Serial.print("MAXs:" + String(reading_max[0]));
	Serial.print(", " + String(reading_max[1]));
	Serial.print(", " + String(reading_max[2]));
	Serial.println(", " + String(reading_max[3]));
}


void printRedings() {
	Serial.print("" + String(reading[0]));
	Serial.print(", " + String(reading[1]));
	Serial.print(", " + String(reading[2]));
	Serial.println(", " + String(reading[3]));

}

void printValues() {
	Serial.print("LL: " + String(reading[0]));
	Serial.print(", L: " + String(reading[1]));
	Serial.print(", P: " + String(reading[2]));
	Serial.print(", PP: " + String(reading[3]));
	Serial.print(", Err: " + String(error));
	Serial.print(", SL: " + String(speed_left));
	Serial.println(", SR: " + String(speed_right));
}