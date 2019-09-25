/*
 * MotorSubfunctions.c
 *
 * Created: 3/6/2018 11:13:08 PM
 *  Author: Zac
 */ 
#include "motorHandler.h"

#include "MotorSubfunctions.h"



BOOL SendNoVerify(SDO_packet Packet, circular_buffer* buffer, int MotorNumber)
{
	char temp[8];
	//if motor is already in error skip comms
	if((MOTORSTATUS & 1<<MotorNumber) == 0)
	{ 
		return FALSE;
	}
	//clear buffer of any already received messages (such as power up message).
	while(cb_size(buffer)>0)
	{
		cb_pop_front(buffer, temp);
	}
	//write data to motor
	while(!SDOWritePacket(Packet));
	return true;
}

//communication functions to verify motors are working as intended.
BOOL SendandVerify(SDO_packet Packet, circular_buffer* buffer, int MotorNumber)
{
	BOOL status = FALSE;
	char temp[8];
	//if motor is already in error skip comms
	if((MOTORSTATUS & 1<<MotorNumber) == 0)
	{
		return FALSE;
	}
	//clear buffer of any already received messages (such as power up message).
	while(cb_size(buffer)>0)
	{
		cb_pop_front(buffer, temp);
	}
	//write data to motor
	while(!SDOWritePacket(Packet));
	//check repeatedly for reply from motor controller
	for(int i = 0; i<COMMSTIMEOUT/COMMSCHECKTIME; i++)
	{
		if(cb_size(buffer)!=0)
		{
			cb_pop_front(buffer, temp);
			if(SDOVerifyReply(temp, Packet))
			{
				status = TRUE;
				break;
			}
		}
		_delay_ms(COMMSCHECKTIME);
	}
	//if nothing received before comms timeout declare motor bad.
	if (!status)
	{
		MOTORSTATUS &= ~(1<<MotorNumber);
	}
	return status;
}

BOOL ReadandVerify(SDO_packet Packet, circular_buffer* buffer, int MotorNumber, void *item)
{
	BOOL status = FALSE;
	char temp[8];
	//if motor is already in error skip comms
	if((MOTORSTATUS & 1<<MotorNumber) == 0)
	{
		return FALSE;
	}
	//clear buffer of any already received messages (such as power up message).
	while(cb_size(buffer)>0)
	{
		cb_pop_front(buffer, temp);
	}
	//write data to motor
	while(!SDOReadRequest(Packet));
	//check repeatedly for reply from motor controller
	for(int i = 0; i<COMMSTIMEOUT/COMMSCHECKTIME; i++)
	{
		if(cb_size(buffer)!=0)
		{
			cb_pop_front(buffer, temp);
			if(SDOVerifyRead(temp, Packet))
			{
				status = TRUE;
				memcpy(item, temp, sizeof(temp));
				break;
			}
		}
		_delay_ms(COMMSCHECKTIME);
	}
	//if nothing received before comms timeout declare motor bad.
	if (!status)
	{
		MOTORSTATUS &= ~(1<<MotorNumber);
	}
	return status;
}

union{
	char byted[4];
	long combined;
	
} assembled;
long ArrayToLong(char temp[8])
{
	assembled.byted[3]=temp[4];
	assembled.byted[2]=temp[5];
	assembled.byted[1]=temp[6];
	assembled.byted[0]=temp[7];
// 	long result = ((((long)temp[5])<<8) + (((long)temp[6]) << 16) + (((long)temp[7]) << 24))+ temp[4] ; //turn LSB format into a long
// 	return result;
return assembled.combined;
}


BOOL getMotorErrorStatus(Motor_t* motor)
{
	return MOTORSTATUS & (1 << motor->Status);
	
}

uint8_t getMotorStatus ( void )
{
	
	return MOTORSTATUS;
}
