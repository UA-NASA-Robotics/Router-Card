/*
 * SDO.c
 *
 * Created: 3/18/2016 10:25:54 PM
 *  Author: reed
 */

#include "SDO.h"
#include "assert.h"
#include "can.h"
#include <avr/io.h>

bool SDOReadRequest( SDO_packet SDOpacket)
{
	CAN_packet SDO = {0x600+SDOpacket.NodeID, 8, "01234567"};
	SDO.data[0] = 0x40;
	SDO.data[1] = SDOpacket.ObjIndx;
	SDO.data[2] = SDOpacket.ObjIndx>>8;
	SDO.data[3] = SDOpacket.SubIndx;
	SDO.data[4] = 0x00;
	SDO.data[5] = 0x00;
	SDO.data[6] = 0x00;
	SDO.data[7] = 0x00;

	return can_tx(SDOMOB, &SDO);
}

void SDOWritePacketFUll(unsigned int NodeID, unsigned int ObjIndx, char SubIndx, unsigned long Data) {
	CAN_packet SDO = {0x600+NodeID, 8, "01234567"};
	SDO.data[0] = 0x23;
	SDO.data[1] = ObjIndx;
	SDO.data[2] = ObjIndx>>8;
	SDO.data[3] = SubIndx;
	SDO.data[4] = Data;
	SDO.data[5] = Data>>8;
	SDO.data[6] = Data>>16;
	SDO.data[7] = Data>>24;

	can_tx(SDOMOB, &SDO);
}

bool SDOWritePacket(SDO_packet SDOpacket) {

	CAN_packet SDO = {0x600+SDOpacket.NodeID, 8, "01234567"};
	SDO.data[0] = 0x23;
	SDO.data[1] = SDOpacket.ObjIndx;
	SDO.data[2] = SDOpacket.ObjIndx>>8;
	SDO.data[3] = SDOpacket.SubIndx;
	SDO.data[4] = SDOpacket.Data;
	SDO.data[5] = SDOpacket.Data>>8;
	SDO.data[6] = SDOpacket.Data>>16;
	SDO.data[7] = SDOpacket.Data>>24;

	return can_tx(SDOMOB, &SDO);
}

bool SDOVerifyReply(char SDOreply[], SDO_packet SDOsent) {
	//reply has 0x60 in the first byte, and the same data in the other overhead bytes
	//Ignore data bytes (4-7)
	if(SDOreply[0] != 0x60) {
		return false;
	}
	if(SDOreply[1] != (0xFF & SDOsent.ObjIndx)) {
		return false;
	}
	if(SDOreply[2] != SDOsent.ObjIndx>>8) {
		return false;
	}
	if(SDOreply[3] != SDOsent.SubIndx) {
		return false;
	}
	return true;
}

bool SDOVerifyRead(char SDOreply[], SDO_packet SDOsent) {
	//reply has 0x60 in the first byte, and the same data in the other overhead bytes
	//Ignore data bytes (4-7)
	if((SDOreply[0] != 0x42) && (SDOreply[0] != 0x43) && (SDOreply[0] != 0x4B) && (SDOreply[0] != 0x4F)) { //could be 0x43 for exactly 4 bytes, 0x4B for 2 bytes, or 0x4F for 1 byte
		return false;
	}
	if(SDOreply[1] != (0xFF & SDOsent.ObjIndx)) {
		return false;
	}
	if(SDOreply[2] != SDOsent.ObjIndx>>8) {
		return false;
	}
	if(SDOreply[3] != SDOsent.SubIndx) {
		return false;
	}
	return true;
}

