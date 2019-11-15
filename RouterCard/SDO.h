#ifndef _SDO_H
#define _SDO_H
#include <stdbool.h>

#define SDOMOB 14

typedef struct
{
	unsigned char NodeID;
	unsigned int ObjIndx;
	unsigned char SubIndx;
	unsigned long Data;
}
SDO_packet;

void SDOWritePacketFull(unsigned int NodeID, unsigned int ObjIndx, char SubIndx, unsigned long Data);
bool SDOWritePacket(SDO_packet SDOpacket);
bool SDOReadRequest(SDO_packet SDOpacket);
bool SDOVerifyReply(char SDOreply[], SDO_packet SDOsent);
bool SDOVerifyRead(char SDOreply[], SDO_packet SDOsent);
//void SDOread( CAN_packet *p, unsigned char mob);

#endif