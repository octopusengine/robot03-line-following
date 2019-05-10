// 
// 
// 

#include "Motor.h"

Motor::Motor() {

	pinMode(LMOTO_FORWARD, OUTPUT); // Lmotor FORWARD
	pinMode(LMOTO_BACKWARD, OUTPUT); // Lmotor BACKWARD

	pinMode(PMOTO_FORWARD, OUTPUT); // Rmotor FORWARD
	pinMode(PMOTO_BACKWARD, OUTPUT); // Rmotor BACKWARD

	ledcSetup(LMOTO_CHAN, PWM_FREQ, PWM_RES); // ESP32 pwm setup
	ledcSetup(PMOTO_CHAN, PWM_FREQ, PWM_RES); // ESP32 pwm setup

	ledcAttachPin(LMOTO_PWM, LMOTO_CHAN); // Lmotor PWM
	ledcAttachPin(PMOTO_PWM, PMOTO_CHAN); // Rmotor PWM

	//Stops the motors from spinning
	setLeft(0);
	setRight(0);
}


//sets the speed of left motor
void Motor::setLeft(int speed) {

	int forward = true;
	if (speed > 255) speed = 255;
	if (speed < -255) speed = -255;
	if (speed < 0) {
		forward = false;
		speed = -speed;
	}

	digitalWrite(LMOTO_FORWARD, forward);
	digitalWrite(LMOTO_BACKWARD, !forward);
	ledcWrite(LMOTO_CHAN, speed);
}

//sets the speed of right motor
void Motor::setRight(int speed) {

	int forward = true;
	if (speed > 255) speed = 255;
	if (speed < -255) speed = -255;
	if (speed < 0) {
		forward = false;
		speed = -speed;
	}

	digitalWrite(PMOTO_FORWARD, forward);
	digitalWrite(PMOTO_BACKWARD, !forward);
	ledcWrite(PMOTO_CHAN, speed);
}

