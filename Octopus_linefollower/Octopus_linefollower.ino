
#include "BluetoothSerial.h"

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
#define PURPLE 255,0,255
#define YELLOW 255,255,0

#define SERIAL_BAUD 250000

//modules
#define PIN_BUTTON PIN_SERVO3
#define PIN_NEOPIXEL PIN_WS

//analog sensors
#define PIN_LL PIN_I36
#define PIN_L PIN_I39
#define PIN_P PIN_I34
#define PIN_PP PIN_I35

#define SENSOR_COUNT 4

int CPS = 10000; //Program cycles per second (used for PID, to have constant behaviour)
int CPS_debug = 100;


int speed_forward = 90;
int speed_correction = 90;
int speed_backOnTrack = 90;

float pK = 1.0; //0.75 - p constant of pid regulator
float dK = 30; //100-120 - d constant of pid regualtor

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

bool flag_running;
bool flag_outOfTrack;
bool flag_lastTrackRight;
bool flag_lastTrackLeft;

void setup() {
	// the minimal speed depends on CYCLES_PER_SECOND defined
	Serial.begin(SERIAL_BAUD);
	//Serial.println("Start");

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
		CPS = CPS_debug;

		neopixel.setColor(WHITE);
		delay(100);
		neopixel.setColor(BLACK);
		delay(100);
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

	//Serial.println("Setup done");
}



void loop() {

	static unsigned long lastCycleMicros = 0;
	static unsigned long lastCPSMicros = 0;

	static unsigned long RCPS = 0; //cycles per second
	static unsigned long ECPS = 0; //empty cycles per second

	neopixel.setColor(BLACK);
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

		flag_running = true;

		while (flag_running) {
			unsigned long mcrs;

			mcrs = micros();
			if (mcrs - lastCycleMicros >= 1000000 / CPS || mcrs < lastCycleMicros) {
				lastCycleMicros = mcrs;

				readTrack();
				apllyCorrection();
				calculateError();
				applyRegulator();
				calculateSpeed();
				if (!debug) {
					setMotors();
				}
				else {
					printValues();
					delay(10);
				}

				RCPS++;
			}
			else {
				ECPS++;
			}

			mcrs = micros();
			if (mcrs - lastCPSMicros >= 1000000 || mcrs < lastCPSMicros) {
				lastCPSMicros = mcrs;
				Serial.println("CPS = " + String(RCPS) + ", ECPS = " + String(ECPS));
				RCPS = ECPS = 0;
			}

			if (button.read()) {
				flag_running = false;
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

	if(debug)
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

	byte min_tolerance = 25; //procent
	byte max_tolerance = 5;

	for (int i = 0; i < SENSOR_COUNT; i++) {

		int min = reading_min[i] + ((reading_max[i] - reading_min[i]) * (min_tolerance / 100.0));
		int max = reading_max[i] + ((reading_max[i] - reading_min[i]) * (max_tolerance / 100.0));

		if (reading[i] > max)
			reading[i] = max;
		if (reading[i] < min)
			reading[i] = min;

		reading[i] = map(reading[i], min, max, 0, 900);
	}

}

void calculateError() {
	int soucet1 = reading[0] * 1 + reading[1] * 2 + reading[2] * 3 + reading[3] * 4;
	int soucet2 = reading[0] + reading[1] + reading[2] + reading[3];

	if (soucet1 < 0) {
		digitalWrite(BUILT_IN_LED, HIGH);
		Serial.println("!!! Zaporny soucet - asi pretekl");
		flag_running = false;
		error = 0.0;
		return;
	}

	if (soucet2 == 0) {
		if (!flag_outOfTrack) {
			neopixel.setColor(PURPLE);
			flag_outOfTrack = true;
		}
		error = 0.0;
		return;
	}

	else {
		if (flag_outOfTrack) {
			neopixel.setColor(GREEN);
			flag_outOfTrack = false;
		}
	}

	error = (double)soucet1 / (double)soucet2;
	error = error - 2.5;
	error = error / 1.5;

	if (reading[0] > 0) {
		flag_lastTrackLeft = true;
		flag_lastTrackRight = false;
	}
	else if (reading[SENSOR_COUNT - 1] > 0) {
		flag_lastTrackLeft = false;
		flag_lastTrackRight = true;
	}
	if ((reading[0] == 0 && reading[1] > 0) || (reading[SENSOR_COUNT - 2] > 0 && reading[SENSOR_COUNT - 1] == 0)) {
		flag_lastTrackLeft = false;
		flag_lastTrackRight = false;
	}

}

void applyRegulator() {
	static double error_last = 0;

	double correction = error;

	//P
	correction = correction * pK;

	//D
	double difference = error - error_last;
	correction = correction + difference * dK;

	error_last = error;

	error = correction;
}

void calculateSpeed() {

	if (flag_outOfTrack) {
		if (flag_lastTrackRight) {
			speed_left = speed_backOnTrack;
			speed_right = -speed_backOnTrack;
		}
		else if (flag_lastTrackLeft) {
			speed_left = -speed_backOnTrack;
			speed_right = speed_backOnTrack;
		}
		else {
			speed_left = speed_forward;
			speed_right = speed_forward;
		}
	}
	else {
		speed_left = speed_forward + error * speed_correction;
		speed_right = speed_forward - error * speed_correction;
	}
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