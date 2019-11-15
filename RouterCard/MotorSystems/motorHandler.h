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
//char  ID;
//char  Status;
//char  MOB;
//int   MAX_RPM;
//int   MAX_CURRENT;
////bool  CANenabled;
//circular_buffer Motor_Buffer;
//
//}Motor_t;

void InitMotor_BG75(Motor_t* Motor, char address, char statusBit,char mob, int maxRPM, int maxCurrent, LimitSwitch_t limitSwitch);
void InitMotor(Motor_t* Motor, char address, char statusBit,char mob, int maxRPM, int maxCurrent, LimitSwitch_t limitSwitch, bool brushless);

void clearMotorErrorStatusReg(Motor_t* motor);

void ReEstablishComms(Motor_t* motor);
//*******************************************************
//------------------------Setters------------------------
//*******************************************************

void setMotorControlMode(Motor_t *motor, unsigned char mode);
void SetMotorLimit(Motor_t *motor);

void setMotorPos(Motor_t *motor, int pos);
void setMotorPosNoSafetyComms(Motor_t *motor, int pos);

void setMotorVel(Motor_t *motor, int Vel);
void setMotorVelNoSafetyComms(Motor_t *motor, int Vel);
void setMotorCurNoSafetyComms(Motor_t *motor, int Cur);

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