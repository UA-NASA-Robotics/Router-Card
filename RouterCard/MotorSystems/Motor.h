/*
 * IncFile1.h
 *
 * Created: 4/4/2016 8:24:18 PM
 *  Author: reed
 */

#ifndef Motor_H_
#define Motor_H_

#include <stdbool.h>
#include "../SDO.h"
#include "../AT90CAN_UART.h"
#include "motorHandler.h"
#include "MotorDefines.h"
#include "MotorSubfunctions.h"


#define MotorsCount   4//5

typedef enum {
	Left=0,
	Right,
	Bucket,
	Arm//,
	// Plow
} Speeds;

int motorSpeeds[MotorsCount];
int prevMotorCommand[MotorsCount];
void MotorsAllStop();
void initMotors();
void motorControl(int leftCommand, int rightCommand, int armCommand, int bucketCommand,int plowCommand);



#endif /* Motor_H_ */