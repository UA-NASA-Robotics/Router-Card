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




void testOpenLoopCommandsLeftAndRight(long distance);
void testOpenLoopCommandsLeftAndRightTurn(long distance);
void initMotors(void);

//I like the idea of using these....
void Motor_Init(int motorID); 
void Motor_VelMode(int motorID);
void Motor_MoveCounts(int motorID, int Counts);
void Motor_PosMode(int motorID);
void Motor_SetVel(int motorID, int Vel);


BOOL BucketMotor_Status();
void BucketMotor_Init();
void BucketMotor_VelMode();
void BucketMotor_PosMode();
void BucketMotor_SetVel(int Vel); 
void BucketMotor_SetVelNoCommsSafety(int Vel);
void BucketMotor_MoveCounts(long Counts); 
void BucketMotor_MoveCountsNoCommsSafety( long Counts);
char BucketMotor_GetTemperature();
char BucketMotor_GetVoltage(); 
long BucketMotor_GetPos();
char BucketMotor_Inputs(); 
void BucketMotor_ReEstablishComms();
//set limits for motors with limit switches attached.
void BucketMotor_SetLimit();

BOOL ArmMotor_Status();
void ArmMotor_Init();
void ArmMotor_SetVel(int Vel);
void ArmMotor_SetVelNoCommsSafety(int Vel);
char ArmMotor_GetTemperature();
char ArmMotor_GetVoltage();
long ArmMotor_GetPos();
void ArmMotor_ReEstablishComms();
void ArmMotor_CurrentMode();
void ArmMotor_SetCurrent(int curr);
void ArmMotor_VelMode();


#endif /* Motor_H_ */