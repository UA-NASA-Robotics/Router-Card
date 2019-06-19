/*
 * Motor.c
 *
 * Created: 4/4/2016 8:24:43 PM
 *  Author: reed
 */

#include "Motor.h"
#include "../general.h"
#include "../assert.h"
#include "../can.h"
#include "../CANFastTransfer.h"
#include "../CommsDefenition.h"
#include "../Config.h"
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include "../Init.h"
#include "../Timer.h"
#include "../LEDs.h"
#include "motorHandler.h"
#include "MotorDefines.h"
void sendMotorMoveCommandLeft(long distance);
void sendMotorMoveCommandRight(long distance);
void returnLeftMotorToVelocity(Motor_t* motor);
void returnRightMotorToVelocity(void);

timer_t delayTimer,smallDelayTimer;
<<<<<<< HEAD
//
//void testOpenLoopCommandsLeftAndRight(long distance)
//{	
	//setLED(6,OFF);			//INDICATE!!!
	//
	////Set a timer interval for use here
	//setTimerInterval(&delayTimer,5000);
	//setTimerInterval(&smallDelayTimer,10);	
	//
	//while(!timerDone(&smallDelayTimer));
	////Put the motors in position mode and move them a distance
	//sendMotorMoveCommandLeft(distance);
	//sendMotorMoveCommandRight(-distance);
	//
	//setLED(6,ON);			//INDICATE!!!
	//while(!(RightMotor_GetPosReached() && LeftMotor_GetPosReached()))
	//{
		//
		//while(!timerDone(&smallDelayTimer));
		//
	//}
	//
	////while(!timerDone(&delayTimer));
	//
	////Return the motors to velocity mode for further use
	//returnLeftMotorToVelocity();
	//returnRightMotorToVelocity();
	//
	//setLED(6,OFF);			//INDICATE!!!
//}
//
////Positive numbers are CW
//void testOpenLoopCommandsLeftAndRightTurn(long distance)
//{	
	//setLED(6,OFF);			//INDICATE!!!
	//setTimerInterval(&delayTimer,10000);
	//setTimerInterval(&smallDelayTimer,10);
	//
	//while(!timerDone(&smallDelayTimer));
	////Put the motors in position mode and move them a distance
	//sendMotorMoveCommandLeft(distance);	
	//sendMotorMoveCommandRight(distance);
	//
	//setLED(6,ON);			//INDICATE!!!
	//while(!(RightMotor_GetPosReached() && LeftMotor_GetPosReached()))
	//{
		//
		//while(!timerDone(&smallDelayTimer));
		//
	//}
	//
	////Return the motors to velocity mode for further use
	//returnLeftMotorToVelocity();
	//returnRightMotorToVelocity();
	//
	//setLED(6,OFF);			//INDICATE!!!
//}
//
//void initMotors(void)
//{
	//
	//
	//
	//
	//
	//#ifdef RIGHT_MOTOR_USING_CAN
	 	//RightMotor_Init();
	 	//RightMotor_VelMode();
		 //RightMotor_SetVel(0);
	//#else
		//ToSendCAN(MOTOR_COMMAND_MODE_DATA_ADDRESS, VELOCITY_MODE);
		//ToSendCAN(MOTOR_VELOCITY_DATA_ADDRESS, 0);												//Works
		//sendDataCAN(RightBumperAddress);
	//#endif
	//
	//#ifdef LEFT_MOTOR_USING_CAN
		//LeftMotor_Init();
		//LeftMotor_VelMode();
		//LeftMotor_SetVel(0);
	//#else
		//ToSendCAN(MOTOR_COMMAND_MODE_DATA_ADDRESS, VELOCITY_MODE);
		//ToSendCAN(MOTOR_VELOCITY_DATA_ADDRESS, 0);												//Works
		//sendDataCAN(LeftBumperAddress);
	//#endif
	//
	//#ifndef DISABLE_CONVEYOR_MOTOR
		//ConveyorMotor_Init();
		//ConveyorMotor_VelMode();
	//#endif
	//
	//#ifndef DISABLE_BUCKET_MOTOR
		//BucketMotor_Init();
		//BucketMotor_VelMode();
		//BucketMotor_SetLimit();
		////BucketMotor_PosMode();
	//#endif
//}
//
//
//
//void sendMotorMoveCommandLeft(long distance)
//{
	//SDO_packet ClearErrorsL			= {LEFTMOTORID, 0x3000, 0x00, 0x01};
	//SDO_packet ModePosL				= {LEFTMOTORID,	0x3003, 0x00, 0x7};
	//SDO_packet SetPositionWindowL	= {LEFTMOTORID,	0x373A, 0x00, 1000};
	//SDO_packet DesiredVelocityL		= {LEFTMOTORID,	0x3300, 0x0,  4000};
	//SDO_packet ResetPositionL		= {LEFTMOTORID,	0x3762, 0x00, 0x00};
	//
	//SendandVerify(ClearErrorsL,			&LeftMotor_Buffer, LEFTMOTOR_STATUS);
	//SendandVerify(SetPositionWindowL,	&LeftMotor_Buffer, LEFTMOTOR_STATUS);
	//SendandVerify(ResetPositionL,		&LeftMotor_Buffer, LEFTMOTOR_STATUS);
	//SendandVerify(ModePosL,				&LeftMotor_Buffer, LEFTMOTOR_STATUS);
	//SendandVerify(DesiredVelocityL,		&LeftMotor_Buffer, LEFTMOTOR_STATUS);
	//
	////-------Send the motor a position step instruction-----------------------
	//SDO_packet MoveL = {LEFTMOTORID, 0x3790, 0x00, distance};
	//SendandVerify(MoveL,  &LeftMotor_Buffer, LEFTMOTOR_STATUS);
//}
//
//void sendMotorMoveCommandRight(long distance)
//{
	//// --------------set motor configurations to be in position mode -----------
	//SDO_packet ClearErrorsR			= {RIGHTMOTORID,	0x3000, 0x00, 0x01};
	//SDO_packet ModePosR				= {RIGHTMOTORID,	0x3003, 0x00, 0x7};
	//SDO_packet SetPositionWindowR	= {RIGHTMOTORID,	0x373A, 0x00, 1000};
	//SDO_packet DesiredVelocityR		= {RIGHTMOTORID,	0x3300, 0x0,  4000};
	//SDO_packet ResetPositionR		= {RIGHTMOTORID,	0x3762, 0x00, 0x00};
	//
	//SendandVerify(ClearErrorsR,			&RightMotor_Buffer, RIGHTMOTOR_STATUS);
	//SendandVerify(SetPositionWindowR,	&RightMotor_Buffer, RIGHTMOTOR_STATUS);
	//SendandVerify(ResetPositionR,		&RightMotor_Buffer, RIGHTMOTOR_STATUS);
	//SendandVerify(ModePosR,				&RightMotor_Buffer, RIGHTMOTOR_STATUS);
	//SendandVerify(DesiredVelocityR,		&RightMotor_Buffer, RIGHTMOTOR_STATUS);
	//
	//
	//SDO_packet MoveR = {RIGHTMOTORID, 0x3790, 0x00, -distance};
	//SendandVerify(MoveR,  &RightMotor_Buffer, RIGHTMOTOR_STATUS);
//}
//
//void returnLeftMotorToVelocity(Motor_t* motor)
//{
	////When finished, set the left motor back to a velocity mode
	//SDO_packet ModeVelL				= {motor->ID, 0x3003, 0x00, 0x3};
	//SDO_packet VEL_AccL				= {motor->ID, 0x3380, 0x00, ACCEL_CONST};
	//SDO_packet VEL_DecL				= {motor->ID, 0x3381, 0x00, ACCEL_CONST};
	//SDO_packet DesiredVelocityLVel	= {motor->ID,	0x3300, 0x0,  0};
	//
	//SendandVerify(ModeVelL,					&LeftMotor_Buffer, motor->STATUS);
	//SendandVerify(VEL_AccL,					&LeftMotor_Buffer, motor->STATUS);
	//SendandVerify(VEL_DecL,					&LeftMotor_Buffer, motor->STATUS);
	//SendandVerify(DesiredVelocityLVel,		&LeftMotor_Buffer, motor->STATUS);
//}
//
//void returnRightMotorToVelocity(void)
//{
	////--------When finished, set the right motor back to a velocity mode-------
	//SDO_packet ModeVelR				= {RIGHTMOTORID, 0x3003, 0x00, 0x3};
	//SDO_packet VEL_AccR				= {RIGHTMOTORID, 0x3380, 0x00, ACCEL_CONST}; //value should be between 100k and 10k
	//SDO_packet VEL_DecR				= {RIGHTMOTORID, 0x3381, 0x00, ACCEL_CONST};
	//SDO_packet DesiredVelocityRVel	= {RIGHTMOTORID, 0x3300, 0x0,  0};
//
	//SendandVerify(ModeVelR,					&RightMotor_Buffer, RIGHTMOTOR_STATUS);
	//SendandVerify(VEL_AccR,					&RightMotor_Buffer, RIGHTMOTOR_STATUS);
	//SendandVerify(VEL_DecR,					&RightMotor_Buffer, RIGHTMOTOR_STATUS);
	//SendandVerify(DesiredVelocityRVel,		&RightMotor_Buffer, RIGHTMOTOR_STATUS);
//}
//
//
//typedef enum{
	//motorOnline,
	//motorClearErrors,
	//motorPowerDisable,
	//motorSetFeedBack,
	//motorSetSVELFeedBack
	////ETC.
//}rebootProcedure_t;
//rebootProcedure_t leftMotorReboot=motorOnline;
=======

void testOpenLoopCommandsLeftAndRight(long distance)
{	
	setLED(6,OFF);			//INDICATE!!!
	
	//Set a timer interval for use here
	setTimerInterval(&delayTimer,5000);
	setTimerInterval(&smallDelayTimer,10);	
	
	while(!timerDone(&smallDelayTimer));
	//Put the motors in position mode and move them a distance
	sendMotorMoveCommandLeft(distance);
	sendMotorMoveCommandRight(-distance);
	
	setLED(6,ON);			//INDICATE!!!
	while(!(RightMotor_GetPosReached() && LeftMotor_GetPosReached()))
	{
		
		while(!timerDone(&smallDelayTimer));
		
	}
	
	//while(!timerDone(&delayTimer));
	
	//Return the motors to velocity mode for further use
	returnLeftMotorToVelocity();
	returnRightMotorToVelocity();
	
	setLED(6,OFF);			//INDICATE!!!
}

//Positive numbers are CW
void testOpenLoopCommandsLeftAndRightTurn(long distance)
{	
	setLED(6,OFF);			//INDICATE!!!
	setTimerInterval(&delayTimer,10000);
	setTimerInterval(&smallDelayTimer,10);
	
	while(!timerDone(&smallDelayTimer));
	//Put the motors in position mode and move them a distance
	sendMotorMoveCommandLeft(distance);	
	sendMotorMoveCommandRight(distance);
	
	setLED(6,ON);			//INDICATE!!!
	while(!(RightMotor_GetPosReached() && LeftMotor_GetPosReached()))
	{
		
		while(!timerDone(&smallDelayTimer));
		
	}
	
	//Return the motors to velocity mode for further use
	returnLeftMotorToVelocity();
	returnRightMotorToVelocity();
	
	setLED(6,OFF);			//INDICATE!!!
}

void initMotors(void)
{
	#ifdef RIGHT_MOTOR_USING_CAN
	 	RightMotor_Init();
	 	RightMotor_VelMode();
		 RightMotor_SetVel(0);
	#else
		ToSendCAN(MOTOR_COMMAND_MODE_DATA_ADDRESS, VELOCITY_MODE);
		ToSendCAN(MOTOR_VELOCITY_DATA_ADDRESS, 0);												//Works
		sendDataCAN(RightBumperAddress);
	#endif
	
	#ifdef LEFT_MOTOR_USING_CAN
		LeftMotor_Init();
		LeftMotor_VelMode();
		LeftMotor_SetVel(0);
	#else
		ToSendCAN(MOTOR_COMMAND_MODE_DATA_ADDRESS, VELOCITY_MODE);
		ToSendCAN(MOTOR_VELOCITY_DATA_ADDRESS, 0);												//Works
		sendDataCAN(LeftBumperAddress);
	#endif
	
	#ifndef DISABLE_ARM_MOTOR
		ArmMotor_Init();
		ArmMotor_VelMode();
	#endif
	
	#ifndef DISABLE_BUCKET_MOTOR
		BucketMotor_Init();
		BucketMotor_VelMode();
		BucketMotor_SetLimit();
		//BucketMotor_PosMode();
	#endif
}



void sendMotorMoveCommandLeft(long distance)
{
	SDO_packet ClearErrorsL			= {LEFTMOTORID, 0x3000, 0x00, 0x01};
	SDO_packet ModePosL				= {LEFTMOTORID,	0x3003, 0x00, 0x7};
	SDO_packet SetPositionWindowL	= {LEFTMOTORID,	0x373A, 0x00, 1000};
	SDO_packet DesiredVelocityL		= {LEFTMOTORID,	0x3300, 0x0,  4000};
	SDO_packet ResetPositionL		= {LEFTMOTORID,	0x3762, 0x00, 0x00};
	
	SendandVerify(ClearErrorsL,			&LeftMotor_Buffer, LEFTMOTOR_STATUS);
	SendandVerify(SetPositionWindowL,	&LeftMotor_Buffer, LEFTMOTOR_STATUS);
	SendandVerify(ResetPositionL,		&LeftMotor_Buffer, LEFTMOTOR_STATUS);
	SendandVerify(ModePosL,				&LeftMotor_Buffer, LEFTMOTOR_STATUS);
	SendandVerify(DesiredVelocityL,		&LeftMotor_Buffer, LEFTMOTOR_STATUS);
	
	//-------Send the motor a position step instruction-----------------------
	SDO_packet MoveL = {LEFTMOTORID, 0x3790, 0x00, distance};
	SendandVerify(MoveL,  &LeftMotor_Buffer, LEFTMOTOR_STATUS);
}

void sendMotorMoveCommandRight(long distance)
{
	// --------------set motor configurations to be in position mode -----------
	SDO_packet ClearErrorsR			= {RIGHTMOTORID,	0x3000, 0x00, 0x01};
	SDO_packet ModePosR				= {RIGHTMOTORID,	0x3003, 0x00, 0x7};
	SDO_packet SetPositionWindowR	= {RIGHTMOTORID,	0x373A, 0x00, 1000};
	SDO_packet DesiredVelocityR		= {RIGHTMOTORID,	0x3300, 0x0,  4000};
	SDO_packet ResetPositionR		= {RIGHTMOTORID,	0x3762, 0x00, 0x00};
	
	SendandVerify(ClearErrorsR,			&RightMotor_Buffer, RIGHTMOTOR_STATUS);
	SendandVerify(SetPositionWindowR,	&RightMotor_Buffer, RIGHTMOTOR_STATUS);
	SendandVerify(ResetPositionR,		&RightMotor_Buffer, RIGHTMOTOR_STATUS);
	SendandVerify(ModePosR,				&RightMotor_Buffer, RIGHTMOTOR_STATUS);
	SendandVerify(DesiredVelocityR,		&RightMotor_Buffer, RIGHTMOTOR_STATUS);
	
	
	SDO_packet MoveR = {RIGHTMOTORID, 0x3790, 0x00, -distance};
	SendandVerify(MoveR,  &RightMotor_Buffer, RIGHTMOTOR_STATUS);
}

void returnLeftMotorToVelocity(void)
{
	//When finished, set the left motor back to a velocity mode
	SDO_packet ModeVelL				= {LEFTMOTORID, 0x3003, 0x00, 0x3};
	SDO_packet VEL_AccL				= {LEFTMOTORID, 0x3380, 0x00, ACCEL_CONST};
	SDO_packet VEL_DecL				= {LEFTMOTORID, 0x3381, 0x00, ACCEL_CONST};
	SDO_packet DesiredVelocityLVel	= {LEFTMOTORID,	0x3300, 0x0,  0};
	
	SendandVerify(ModeVelL,					&LeftMotor_Buffer, LEFTMOTOR_STATUS);
	SendandVerify(VEL_AccL,					&LeftMotor_Buffer, LEFTMOTOR_STATUS);
	SendandVerify(VEL_DecL,					&LeftMotor_Buffer, LEFTMOTOR_STATUS);
	SendandVerify(DesiredVelocityLVel,		&LeftMotor_Buffer, LEFTMOTOR_STATUS);
}

void returnRightMotorToVelocity(void)
{
	//--------When finished, set the right motor back to a velocity mode-------
	SDO_packet ModeVelR				= {RIGHTMOTORID, 0x3003, 0x00, 0x3};
	SDO_packet VEL_AccR				= {RIGHTMOTORID, 0x3380, 0x00, ACCEL_CONST}; //value should be between 100k and 10k
	SDO_packet VEL_DecR				= {RIGHTMOTORID, 0x3381, 0x00, ACCEL_CONST};
	SDO_packet DesiredVelocityRVel	= {RIGHTMOTORID, 0x3300, 0x0,  0};

	SendandVerify(ModeVelR,					&RightMotor_Buffer, RIGHTMOTOR_STATUS);
	SendandVerify(VEL_AccR,					&RightMotor_Buffer, RIGHTMOTOR_STATUS);
	SendandVerify(VEL_DecR,					&RightMotor_Buffer, RIGHTMOTOR_STATUS);
	SendandVerify(DesiredVelocityRVel,		&RightMotor_Buffer, RIGHTMOTOR_STATUS);
}


typedef enum{
	motorOnline,
	motorClearErrors,
	motorPowerDisable,
	motorSetFeedBack,
	motorSetSVELFeedBack
	//ETC.
}rebootProcedure_t;
rebootProcedure_t leftMotorReboot=motorOnline;
>>>>>>> develop
