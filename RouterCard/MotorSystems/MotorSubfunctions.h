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
bool SendNoVerify(SDO_packet Packet, circular_buffer* buffer, int MotorNumber);
bool SendandVerify(SDO_packet Packet, circular_buffer* buffer, int MotorNumber);
bool ReadandVerify(SDO_packet Packet, circular_buffer* buffer, int MotorNumber, void *item);
long ArrayToLong(char temp[8]);

void setMotorStatus(uint8_t stat);
bool getMotorErrorStatus(Motor_t* motor);

#endif /* MOTORSUBFUNCTIONS_H_ */