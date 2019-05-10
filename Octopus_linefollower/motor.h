// Motor.h

#ifndef _MOTOR_h
#define _MOTOR_h

#include "octopuslibrary.h"

#define PMOTO_FORWARD PIN_MOTOR1A
#define PMOTO_BACKWARD PIN_MOTOR2A
#define PMOTO_PWM PIN_MOTOR12EN

#define LMOTO_FORWARD PIN_MOTOR4A
#define LMOTO_BACKWARD PIN_MOTOR3A
#define LMOTO_PWM PIN_MOTOR34EN

#define PWM_FREQ 500 //this can be almost whatever you want
#define PWM_RES 8 //no need to be more precise
#define LMOTO_CHAN 0
#define PMOTO_CHAN 1

class Motor
{
public:
	Motor();
	void setLeft(int speed);
	void setRight(int speed);
};

#endif

