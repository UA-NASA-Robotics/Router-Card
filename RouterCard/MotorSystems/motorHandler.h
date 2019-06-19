/*
 * motorHandle.h
 *
 * Created: 4/7/2018 12:42:19 PM
 *  Author: sjc93
 */ 


#ifndef MOTORHANDLER_H
#define MOTORHANDLER_H

#include "..\Config.h"
#include "MotorDefines.h"
#include "MotorSubfunctions.h"
//typedef enum
//{
	//Velocity=0,
	//Position
	//
//}MotorMode;
//
//
//
//typedef struct 
//{
	////Can Address
	//char 	ID;
	//char 	Status;
	//char 	MOB;
	//int 	MAX_RPM;
	//int 	MAX_CURRENT;
	////bool 	CANenabled;
	//circular_buffer Motor_Buffer;
	//
//}Motor_t;


void InitMotor(Motor_t* Motor, char address, char statusBit,char mob, int maxRPM, int maxCurrent);

void clearMotorErrorStatusReg(Motor_t* motor);

void ReEstablishComms(Motor_t* motor);
//*******************************************************
//------------------------Setters------------------------
//*******************************************************

void setMotorControlMode(Motor_t *motor, unsigned char mode);

void setMotorPos(Motor_t *motor, int pos);
void setMotorPosNoSafetyComms(Motor_t *motor, int pos);

void setMotorVel(Motor_t *motor, int Vel);
void setMotorVelNoSafetyComms(Motor_t *motor, int Vel);


void setMotorCounts(Motor_t* motor, long counts);
void setMotorCountsNoSafetyComms(Motor_t *motor, long counts);

//*******************************************************
//------------------------Getters------------------------
//*******************************************************

long getMotorPos(Motor_t *motor);
char getMotorVoltage(Motor_t *motor);
char getMotorTemp(Motor_t *motor);
bool getMotorPosReached(Motor_t *motor);
//bool getMotorStatus(Motor_t *motor);
char getMotorDigital(Motor_t *motor);
int getMotorAnalog(Motor_t *motor);

#endif