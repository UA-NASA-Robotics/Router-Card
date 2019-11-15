/*
 * Motor.c
 *
 * Created: 4/4/2016 8:24:43 PM
 *  Author: reed
 */

#include "Motor.h"
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


timer_t resendMotorTimer[5];


void initMotors()
{


	//motor status tracking
	MOTORSTATUS = 0xFF; //assume all motors are innocent until proven guilty
	for(int i = 0; i < 5; i ++)
		setTimerInterval(&resendMotorTimer[i],50);

#ifndef DISABLE_LEFT_MOTOR
	//LeftMotor = (Motor_t){LEFTMOTORID,LEFTMOTOR_STATUS,LEFTMOTOR_MOB,MAXRPM,MAXCURRENTBG65,ACCEL_CONST,true,(LimitSwitch_t){0,0},LeftMotor.Motor_Buffer};
	InitMotor(&LeftMotor, LEFTMOTORID, LEFTMOTOR_STATUS, LEFTMOTOR_MOB, MAXRPM, MAXCURRENTBG65,(LimitSwitch_t) {
		0,0
	}, true);
//  setMotorControlMode(&LeftMotor, Velocity);
//  setMotorVel(&LeftMotor, 0);
#endif /*DISABLE_LEFT_MOTOR*/

#ifndef DISABLE_RIGHT_MOTOR
	InitMotor(&RightMotor, RIGHTMOTORID, RIGHTMOTOR_STATUS, RIGHTMOTOR_MOB, MAXRPM, MAXCURRENTBG65,(LimitSwitch_t) {
		0,0
	},true);
//  setMotorControlMode(&RightMotor, Velocity);
//  setMotorVel(&RightMotor, 0);
#endif /*DISABLE_ARM_MOTOR*/

#ifndef DISABLE_ARM_MOTOR
	InitMotor(&ArmMotor, ARMMOTORID, ARMMOTOR_STATUS, ARMMOTOR_MOB, MAXRPM, MAXCURRENTARM, (LimitSwitch_t) {
		LIM_D0H,LIM_D1H
		,0
	}, false);
//  setMotorControlMode(&ArmMotor, Velocity);
//  setMotorVel(&ArmMotor, 0);
#endif /*DISABLE_LEFT_MOTOR*/

#ifndef DISABLE_BUCKET_MOTOR
	InitMotor(&BucketMotor, BUCKETMOTORID, BUCKETMOTOR_STATUS, BUCKETMOTOR_MOB, MAXRPM, MAXCURRENTBUCKET,(LimitSwitch_t) {
		0,0
	},true);
//  setMotorControlMode(&BucketMotor, Velocity);
//  setMotorVel(&BucketMotor, 0);
#endif /*DISABLE_LEFT_MOTOR*/

#ifndef DISABLE_PLOW_MOTOR
	InitMotor(&PlowMotor, PLOWMOTORID, PLOWMOTOR_STATUS, PLOWMOTOR_MOB, MAXRPM, MAXCURRENTPLOW,(LimitSwitch_t) {
		0,0
	},false);
//  setMotorControlMode(&PlowMotor, Velocity);
//  setMotorVel(&PlowMotor, 0);
#endif /*DISABLE_LEFT_MOTOR*/

}

#define DRIVESCALER 50
void motorControl(int leftCommand, int rightCommand, int armCommand, int bucketCommand,int plowCommand)
{
	motorSpeeds[Left] = leftCommand;
	motorSpeeds[Right] = rightCommand;
	motorSpeeds[Bucket] = bucketCommand;
	motorSpeeds[Arm] = armCommand;
	//motorSpeeds[Plow] = plowCommand;


//  #ifndef DISABLE_MOTOR_SYSTEMS
//  if(motorSpeeds[Arm] > MANUAL_MIN_SPEED)
//    setLED(10,ON);
//  else
//    setLED(10, OFF);
//    #endif
#ifndef DISABLE_MOTOR_SYSTEMS
#ifndef DISABLE_LEFT_MOTOR

	//*********************************DRIVE SYSTEM UPDATE**********************************
	int m;
	for(m = Left; m <= Right; m++)
	{
//      if(abs(motorSpeeds[m])>MANUAL_MIN_SPEED)
//      {
//        //printf("R Speed: %d\r\n", getRightMotorCommand());
//        setLED(m+10,ON);
//      }
//      else
//      {
//        setLED(m+10,OFF);
//        motorSpeeds[m]=(0);
//      }
		//If the current motor command is not the same as the last sent
		if((prevMotorCommand[m]!=motorSpeeds[m])|| timerDone(&resendMotorTimer[Right]))
		{
			if(abs(motorSpeeds[m]) > MANUAL_MIN_SPEED)
			{


				//Send the right motor speed control
				if(m==Left)
				{

#ifdef REVERSE_LEFT_RIGHT
					setMotorVelNoSafetyComms(&LeftMotor, -1*DRIVESCALER*(motorSpeeds[m]));
#else
					setMotorVelNoSafetyComms(&LeftMotor, DRIVESCALER*(motorSpeeds[m]));
#endif

				}
				else if(m==Right)
				{
#ifdef REVERSE_LEFT_RIGHT
					setMotorVelNoSafetyComms(&RightMotor, DRIVESCALER*(motorSpeeds[m]));
#else
					setMotorVelNoSafetyComms(&RightMotor, -1*DRIVESCALER*(motorSpeeds[m]));
#endif
				}

				//Store value previously sent
				prevMotorCommand[m]=motorSpeeds[m];
				resetTimer(&resendMotorTimer[Right]);
			}
			else
			{

				setMotorVelNoSafetyComms(&LeftMotor,0);
				setMotorVelNoSafetyComms(&RightMotor,0);


			}
		}
	}
#endif
	//*********************************ARM MOTOR UPDATE**********************************
#ifndef DISABLE_ARM_MOTOR
	//If the current motor command is not the same as the last sent
	if(prevMotorCommand[Arm]!=motorSpeeds[Arm] || timerDone(&resendMotorTimer[Arm]))
	{
		//SET ARM MOTOR COMMANDS
		if(motorSpeeds[Arm] >MANUAL_MIN_SPEED)
		{

			setMotorCurNoSafetyComms(&ArmMotor,motorSpeeds[Arm]*40);

		}
		else if(motorSpeeds[Arm] < -MANUAL_MIN_SPEED)
		{

			setMotorCurNoSafetyComms(&ArmMotor,motorSpeeds[Arm]*55);
			//setLED(9,ON);
		}
		else
		{
			setMotorCurNoSafetyComms(&ArmMotor,0);
		}
		//Store value previously sent
		prevMotorCommand[Arm]=motorSpeeds[Arm];
		resetTimer(&resendMotorTimer[Arm]);
	}
#endif /* DISABLE_ARM_MOTOR */

	//*********************************BUCKET MOTOR UPDATE**********************************
#ifndef DISABLE_BUCKET_MOTOR
	//If the current motor command is not the same as the last sent
	if(prevMotorCommand[Bucket]!=motorSpeeds[Bucket]|| timerDone(&resendMotorTimer[Bucket]))
	{

		//SET ARM MOTOR COMMANDS
		if(abs(motorSpeeds[Bucket]) > MANUAL_MIN_SPEED)
		{
			setMotorVelNoSafetyComms(&BucketMotor,motorSpeeds[Bucket]*50);
			//setLED(9,ON);

//    }
//    else if(motorSpeeds[Bucket] > MANUAL_MIN_SPEED)
//    {
//      setMotorVelNoSafetyComms(&BucketMotor, motorSpeeds[Bucket]);
		}
		else
		{
			setMotorVelNoSafetyComms(&BucketMotor,0);
			//setLED(9,OFF);

		}

		//Store value previously sent
		prevMotorCommand[Bucket]=motorSpeeds[Bucket];
		resetTimer(&resendMotorTimer[Bucket]);
	}
#endif /* DISABLE_BUCKET_MOTOR */
#ifndef DISABLE_PLOW_MOTOR
	if(prevMotorCommand[Plow]!=motorSpeeds[Plow]|| timerDone(&resendMotorTimer[Plow]))
	{

		//SET ARM MOTOR COMMANDS
		if(motorSpeeds[Plow] >MANUAL_MIN_SPEED)
		{
			setMotorVelNoSafetyComms(&PlowMotor,2000);
		}
		else if(motorSpeeds[Plow] < -MANUAL_MIN_SPEED)
		{
			setMotorVelNoSafetyComms(&PlowMotor, -2000);
		}
		else
		{
			setMotorVelNoSafetyComms(&PlowMotor,0);
		}
		//if(getMotorErrorStatus(&PlowMotor))
		//{
		//toggleLED(10);
		//}else{
		//toggleLED(11);
		//}
		//Store value previously sent
		prevMotorCommand[Plow]=motorSpeeds[Plow];
		resetTimer(&resendMotorTimer[Plow]);
	}
#endif /*DISABLE_PLOW_MOTOR*/
#endif /* DISABLE_MOTOR_SYSTEMS */
	//TODO:

}
void MotorsAllStop()
{

#ifndef DISABLE_MOTOR_SYSTEMS

	int m;
	for(m = Left; m < Right; m++)
	{
		//setLED(m+11,OFF);
		motorSpeeds[m]=(0);

		//Send motor STOP
		setMotorVelNoSafetyComms((m==Left? &LeftMotor:&RightMotor), 0);

		//Store value previously sent
		prevMotorCommand[m]=motorSpeeds[m];
	}

#ifndef DISABLE_BUCKET_MOTOR
	setMotorVelNoSafetyComms(&BucketMotor,0);
#endif

#ifndef DISABLE_ARM_MOTOR
	setMotorVelNoSafetyComms(&ArmMotor, 0);
#endif

#ifndef DISABLE_PLOW_MOTOR
	setMotorVelNoSafetyComms(&PlowMotor, 0);
#endif

#endif
}
