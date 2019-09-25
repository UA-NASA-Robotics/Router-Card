/*
 * MotorSubfunctions.h
 *
 * Created: 3/6/2018 11:13:20 PM
 *  Author: Zac
 */ 


#ifndef MOTORSUBFUNCTIONS_H_
#define MOTORSUBFUNCTIONS_H_

#include "MotorDefines.h"




//communication functions to protect comms to motors.
BOOL SendNoVerify(SDO_packet Packet, circular_buffer* buffer, int MotorNumber);
BOOL SendandVerify(SDO_packet Packet, circular_buffer* buffer, int MotorNumber);
BOOL ReadandVerify(SDO_packet Packet, circular_buffer* buffer, int MotorNumber, void *item);
long ArrayToLong(char temp[8]);

void setMotorStatus(uint8_t stat);
BOOL getMotorErrorStatus(Motor_t* motor);

#endif /* MOTORSUBFUNCTIONS_H_ */