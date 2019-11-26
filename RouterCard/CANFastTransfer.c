
/*
 * CANFastTransfer.c
 *
 * Created: 10/26/2016 11:23:36 PM
 *  Author: reed
 */
#include <avr/io.h>
#include "Definitions.h"
#include "CANFastTransfer.h"
#include "can.h"
#include "assert.h"
#include "PointerList.h"
#include "CommsDefenition.h"
#include "QueueCANtoUART.h"
#include "AT90CAN_UART.h"
#include "Config.h"
#include "LEDs.h"
#include <avr/delay.h>
#include "Config.h"
#include <stdlib.h>
#define Instant  0
#define WhenReceiveCall  1




int receiveMode = 0;
struct Node * head = NULL;
struct ringBufSCAN ReceiveBuffer;
struct ringBufSCAN ReceiveBufferControl;
struct ringBufSCAN ReceiveBufferBeacon;
struct ringBufSCAN ring_buffer_CAN_Control;
struct ringBufSCAN ring_buffer_CAN_Beacon;
int ReceivedData = 0;
int TransmitSetMissMatch = 0;





void initCANFastTransfer(void)
{
	beginCANFast(ReceiveCAN, GLOBAL_DEVICES*GLOBAL_DATA_IDX_PER_DEV/*sizeof(ReceiveCAN)*/, ROUTER_ADDRESS);

}
//Start Receive Functions

void ReceiveCANFast( CAN_packet *p, unsigned char mob) // interrupt callback
{
	//Possibly two modes one that requires calling receive data like old Fast Transfer for compatibility purposes
	// Then the second that automatically updates the array, and is more efficient.

	if(receiveMode == WhenReceiveCall)
	{
		if(p->length == 8 || p->length == 9) //Check number of bytes, if 8 read in two integers
		{
			if((p->data[0]<<8) +(p->data[1]) < MaxIndex) {
				Send_buffer_put(&ReceiveBuffer, (p->data[0]<<8) +(p->data[1]), (p->data[2]<<8) +(p->data[3]));
				ReceivedData = 1;
			}
			if((p->data[4]<<8) +(p->data[5]) < MaxIndex) {
				Send_buffer_put(&ReceiveBuffer, (p->data[4]<<8) +(p->data[5]), (p->data[6]<<8) +(p->data[7]));
				ReceivedData = 1;
			}
			Send_buffer_put(&ReceiveBuffer, LastBoardReceived, (p->id & 0b11111));
		}
		else //else read in one int
		{
			if((p->data[0]<<8) +(p->data[1]) < MaxIndex) {
				Send_buffer_put(&ReceiveBuffer, (p->data[0]<<8) +(p->data[1]), (p->data[2]<<8) +(p->data[3]));
				ReceivedData = 1;
			}
			Send_buffer_put(&ReceiveBuffer, LastBoardReceived, (p->id & 0b11111));
		}
		//I'll work on this one later
		//basic idea is to dump all data into a circular buffer with no processing and deal with it receiveDataCAN();
	} //end wait receive mode
	else //instant
	{


		bool isGlobal = false;
		if((p->id >> 6) & (0b11111) == GLOBAL_CAN_ADDRESS)
			isGlobal = true;
		//receiveArrayAddressCAN[LastBoardReceived]= (p->id & 0b11111); //set last board received
		setCANFTdata(LastBoardReceived,  (p->id & 0b11111),isGlobal);
		if(p->length == 8 || p->length == 9) //Check number of bytes, if 8 read in two ints
		{
			if((p->data[0]<<8) +(p->data[1]) < MaxIndex) {
				//receiveArrayAddressCAN[(p->data[0]<<8) +(p->data[1])] = (p->data[2]<<8) +(p->data[3]);
				setCANFTdata((p->data[0]<<8) +(p->data[1]), (p->data[2]<<8) +(p->data[3]),isGlobal);
				ReceivedData = 1;
			}
			if((p->data[4]<<8) +(p->data[5]) < MaxIndex) {
				//receiveArrayAddressCAN[(p->data[4]<<8) +(p->data[5])] = (p->data[6]<<8) +(p->data[7]);
				setCANFTdata((p->data[4]<<8) +(p->data[5]), (p->data[6]<<8) +(p->data[7]),isGlobal);
				ReceivedData = 1;
			}

		}
		else //else read in one int
		{
			if((p->data[0]<<8) +(p->data[1]) < MaxIndex) {
				//receiveArrayAddressCAN[(p->data[0]<<8) +(p->data[1])] = (p->data[2]<<8) +(p->data[3]);
				setCANFTdata((p->data[0]<<8) +(p->data[1]), (p->data[2]<<8) +(p->data[3]),isGlobal);
				ReceivedData = 1;
			}
		}

	} //end default receive mode

}
//
// #ifndef DISABLE_CAN_FORWARDING_RECEIVEISR
// void ReceiveCANFastCONTROL( CAN_packet *p, unsigned char mob) // interrupt callback
// {
//  //basic idea is to dump all data into a circular buffer
//  //when a packet with length 4 or 9 is received kick data in buffer
//  //to be sent of UART to correct address.
//  if(p->length == 8) //Check number of bytes, if 8 read in two ints
//  {
//    Send_Queue_ControlBox_put(p->id & 0b11111, p->data[1], (p->data[2]<<8) +(p->data[3]));
//    if(p->data[0] != 0) {
//      //handle error trying to transmit too large of an index
//    }
//    Send_Queue_ControlBox_put(p->id & 0b11111, p->data[5], (p->data[6]<<8) +(p->data[7]));
//    if(p->data[4] != 0) {
//      //handle error trying to transmit too large of an index
//    }
//  }
//  else if(p->length == 9) //Check number of bytes, if 9 read in two ints
//  {
//    Send_Queue_ControlBox_put(p->id & 0b11111, p->data[1], (p->data[2]<<8) +(p->data[3]));
//    if(p->data[0] != 0) {
//      //handle error trying to transmit too large of an index
//    }
//    Send_Queue_ControlBox_put(p->id & 0b11111, p->data[5], (p->data[6]<<8) +(p->data[7]));
//    if(p->data[4] != 0) {
//      //handle error trying to transmit too large of an index
//    }
//    //add who sent data to end of packet
//    Send_Queue_ControlBox_put(p->id & 0b11111, LastBoardReceived, p->id & 0b11111);
//    Send_Queue_ControlBox_send(p->id & 0b11111);
//  }
//  else //else read in one int
//  {
//    Send_Queue_ControlBox_put(p->id & 0b11111, p->data[1], (p->data[2]<<8) +(p->data[3]));
//    if(p->data[0] != 0) {
//      //handle error trying to transmit too large of an index
//    }
//    Send_Queue_ControlBox_put(p->id & 0b11111, LastBoardReceived, p->id & 0b11111);
//    Send_Queue_ControlBox_send(p->id & 0b11111);
//  }
// }
// void ReceiveCANFastBEACON( CAN_packet *p, unsigned char mob) // interrupt callback
// {
//  //basic idea is to dump all data into a circular buffer
//  //when a packet with length 4 or 9 is received kick data in buffer
//  //to be sent of UART to correct address.
//  if(p->length == 8) //Check number of bytes, if 8 read in two ints
//  {
//    if((p->data[0]<<8) +(p->data[1]) < MaxIndex) {
//      Send_buffer_put(&ReceiveBufferBeacon, (p->data[0]<<8) +(p->data[1]), (p->data[2]<<8) +(p->data[3]));
//      ReceivedData = 1;
//    }
//    if((p->data[4]<<8) +(p->data[5]) < MaxIndex) {
//      Send_buffer_put(&ReceiveBufferBeacon, (p->data[4]<<8) +(p->data[5]), (p->data[6]<<8) +(p->data[7]));
//      ReceivedData = 1;
//    }
//    Send_buffer_put(&ReceiveBufferBeacon, LastBoardReceived, (p->id & 0b11111));
//  }
//  else if(p->length == 9) //Check number of bytes, if 9 read in two ints
//  {
//    if((p->data[0]<<8) +(p->data[1]) < MaxIndex) {
//      Send_buffer_put(&ReceiveBufferBeacon, (p->data[0]<<8) +(p->data[1]), (p->data[2]<<8) +(p->data[3]));
//      ReceivedData = 1;
//    }
//    if((p->data[4]<<8) +(p->data[5]) < MaxIndex) {
//      Send_buffer_put(&ReceiveBufferBeacon, (p->data[4]<<8) +(p->data[5]), (p->data[6]<<8) +(p->data[7]));
//      ReceivedData = 1;
//    }
//    Send_buffer_put(&ReceiveBufferBeacon, LastBoardReceived, (p->id & 0b11111));
//  }
//  else //else read in one int
//  {
//    if((p->data[0]<<8) +(p->data[1]) < MaxIndex) {
//      Send_buffer_put(&ReceiveBufferBeacon, (p->data[0]<<8) +(p->data[1]), (p->data[2]<<8) +(p->data[3]));
//      ReceivedData = 1;
//    }
//    Send_buffer_put(&ReceiveBufferBeacon, LastBoardReceived, (p->id & 0b11111));
//  }
// }
// #endif

void TransmitCANFast( CAN_packet *p, unsigned char mob) // interrupt callback
{

	//NEW STATIC METHOD
	//if there is something to send

	if(Transmit_buffer_GetCount(&TransmitBuffer) > 0)
	{
//    #ifdef NEW_MAIN
//    toggleLED(LED1);
//    #endif
		//if more than 2 data/index pairs left might be able to send large packet.
		if(Transmit_buffer_GetCount(&TransmitBuffer)>6) {
			unsigned int address = Transmit_buffer_get(&TransmitBuffer);
			p->id = ( address << 6) + ROUTER_ADDRESS; //not passed through messages will have wrong sender address
			//we are good to send the first index/value pair for sure.
			for(int i = 0; i<2; i++) {
				unsigned int temp = Transmit_buffer_get(&TransmitBuffer);
				p->data[2*i] = (temp >> 8);
				p->data[2*i + 1] = temp;
			}
			//now we need to check that the next address matches the current address
			if (Transmit_buffer_peek(&TransmitBuffer) == address) {
				//address match and we are clear to send them in a single packet
				Transmit_buffer_get(&TransmitBuffer); //call this to clear out the address
				for(int i = 2; i<4; i++) {
					unsigned int temp = Transmit_buffer_get(&TransmitBuffer);
					p->data[2*i] = (temp >> 8);
					p->data[2*i + 1] = temp;
				}
				//need to check if this was the last packet
				if(Transmit_buffer_peek(&TransmitBuffer) == address) {
					//next address is same, so this isn't the last packet, send a length 8.
					p->length = 8;
				}
				else {
					//next address is different, and this is the last packet, so send a 9.
					p->length = 9;
				}
			}
			else {
				//addresses don't match and we should just send what is currently in the packet.
				p->length = 4;
			}
		}
		//if exactly 2 data/index pairs left send with length 9. Receiver
		//will read the "wrong" length correctly, but realize this is the last packet.
		//note: still need to check incase two different destinations.
		else if(Transmit_buffer_GetCount(&TransmitBuffer)==6) {
			unsigned int address = Transmit_buffer_get(&TransmitBuffer);
			p->id = ( address << 6) + ROUTER_ADDRESS; //not passed through messages will have wrong sender address
			//we are good to send the first index/value pair for sure.
			for(int i = 0; i<2; i++) {
				unsigned int temp = Transmit_buffer_get(&TransmitBuffer);
				p->data[2*i] = (temp >> 8);//this is so dumb
				p->data[2*i + 1] = temp;
			}
			//now we need to check that the next address matches the current address
			if (Transmit_buffer_peek(&TransmitBuffer) == address) {
				//address match and we are clear to send them in a single packet
				Transmit_buffer_get(&TransmitBuffer); //call this to clear out the address
				for(int i = 2; i<4; i++) {
					unsigned int temp = Transmit_buffer_get(&TransmitBuffer);
					p->data[2*i] = (temp >> 8);
					p->data[2*i + 1] = temp;
				}
				p->length = 9;
			}
			else {
				//addresses don't match and we should just send what is currently in the packet.
				p->length = 4;
			}

		}
		//if only 1 data/index pair receiver will know it is the last packet.
		else if(Transmit_buffer_GetCount(&TransmitBuffer)==3) {
			unsigned int address = Transmit_buffer_get(&TransmitBuffer);
			p->id = ( address << 6) + ROUTER_ADDRESS; //not passed through messages will have wrong sender address
			p->length = 4;
			for(int i = 0; i<2; i++) {
				unsigned int temp = Transmit_buffer_get(&TransmitBuffer);
				p->data[2*i] = (temp >> 8);
				p->data[2*i + 1] = temp;
			}

		}

		else
		{
			//error, missing dest/index/value set, previous packets could be very corrupt
			TransmitSetMissMatch++;
//      #ifdef NEW_MAIN
//        toggleLED(LED9);
//      #endif
			Transmit_buffer_flush(&TransmitBuffer,1);
		}
	}
	else {
		p->id = 1;
		p->length = 0;
	}
}



void beginCANFast(volatile int * ptr, unsigned int maxSize, unsigned char givenAddress) {
	receiveArrayAddressCAN = ptr;
	moduleAddressCAN = givenAddress;
	MaxIndex = maxSize;

	bool ret;
	ret=prepare_rx( CANFAST_MOB, moduleAddressCAN<<6, 0b11111100000, ReceiveCANFast); // all 1s forces comparison
	ret+=prepare_rx(CONTROL_MOB,GLOBAL_CAN_ADDRESS<<6, 0b11111100000,ReceiveCANFast); // Initialization the Receiver for the Global Bus
	ASSERT( ret==0);

	prepare_tx(TRANSMITMOB, 0b11111111111, 0b11111111111, TransmitCANFast);
	List_Init(&head);
	Send_buffer_flush(&ring_buffer_CAN,0);
	//Send_buffer_flush(&ring_buffer_CAN_Control,1);
	//Send_buffer_flush(&ring_buffer_CAN_Beacon,1);
	Transmit_buffer_flush(&TransmitBuffer,0);
}

void SetReceiveMode(int input) {
	if(input == Instant || input == WhenReceiveCall )
	{
		receiveMode = input;
	}
}
void setCANFTdata(int index, int val,bool isGlobal)
{
	if(isGlobal)
	{
		receiveArrayCAN_Global[index ] = val;
		GBL_CAN_FT_recievedFlag[index]=true;
		//printf("rx: %d\n",index);

	} else {

		receiveArrayAddressCAN[index]=val;
		CAN_FT_recievedFlag[index] = true;
	}

}
int getCANFTdatas(int index,bool _isGlobal)
{
	if(_isGlobal)
		return receiveArrayCAN_Global[index];
	else
		return receiveArrayAddressCAN[index];

}
void clearCANFTdataIndex(int index)
{
	receiveArrayAddressCAN[index] = 0;
}
bool getCANFT_Flag(int *receiveArray, int index)
{
	if(receiveArray[index] == false) {
		return false;
	} else {
		receiveArray[index] = false;
		return true;
	}
}
int ReceiveDataCAN(void) {

	if(ReceivedData) {
		ReceivedData = 0;

		int i = Send_buffer_GetCount(&ReceiveBuffer);
		if(i || !receiveMode) //this better be true ... if not in instant receive
		{

			for(; i>0; i=i-2)
			{
				int address = Send_buffer_get(&ReceiveBuffer);
				receiveArrayAddressCAN[address] = Send_buffer_get(&ReceiveBuffer);
				CAN_FT_recievedFlag[address] = true;
				//if(address == 3) {

				//}

			}

			return 1;
		}
		else
		{
			//error (how was ReceiveData true if no data available)
			return 0;
		}

	}
	else
		return 0;
}

//End Receive Functions

//Start Transmit Functions

void ToSendCAN(unsigned int where, unsigned int what)
{
	Send_buffer_put(&ring_buffer_CAN, where, what);
}

void ToSendCAN_Control(unsigned char where, unsigned int what)
{
	Send_buffer_put(&ring_buffer_CAN_Control, where, what);
}
void ToSendCAN_Beacon(unsigned char where, unsigned int what)
{
	Send_buffer_put(&ring_buffer_CAN_Beacon, where, what);
}

void sendDataCAN( unsigned int whereToSend)
{
	//NEW STATIC METHOD
	char TxKickNeeded = 1; //assume kick is needed
	int temp = Send_buffer_GetCount(&ring_buffer_CAN); //get size of things to send
	//USART0_put_C(Transmit_buffer_GetCount(&TransmitBuffer));
	if(Transmit_buffer_GetCount(&TransmitBuffer)>1) {
		TxKickNeeded = 0;
	}
	for(int i = 0; i<(temp>>1); i++) { //need to divide by two since reading index/value pairs, hence >>1
		int index = Send_buffer_get(&ring_buffer_CAN);
		int value = Send_buffer_get(&ring_buffer_CAN);
		Transmit_buffer_put(&TransmitBuffer, whereToSend, index, value);
	}
	if(TxKickNeeded) {
		can_tx_kick(TRANSMITMOB);
	}
}

void sendDataCAN_Control( unsigned int whereToSend)
{
	//NEW STATIC METHOD
	char TxKickNeeded = 1; //assume kick is needed
	int temp = Send_buffer_GetCount(&ring_buffer_CAN_Control); //get size of things to send

	if(Transmit_buffer_GetCount(&TransmitBuffer)>1) {
		TxKickNeeded = 0;
	}
	for(int i = 0; i<(temp>>1); i++) { //need to divide by two since reading index/value pairs, hence >>1
		int index = Send_buffer_get(&ring_buffer_CAN_Control);
		int value = Send_buffer_get(&ring_buffer_CAN_Control);
		Transmit_buffer_put(&TransmitBuffer, whereToSend, index, value);
	}
	if(TxKickNeeded) {
		can_tx_kick(TRANSMITMOB);
	}

}

void sendDataCAN_Beacon( unsigned int whereToSend)
{
	//NEW STATIC METHOD
	char TxKickNeeded = 1; //assume kick is needed
	int temp = Send_buffer_GetCount(&ring_buffer_CAN_Beacon); //get size of things to send

	if(Transmit_buffer_GetCount(&TransmitBuffer)>1) {
		TxKickNeeded = 0;
	}
	for(int i = 0; i<(temp>>1); i++) { //need to divid by two since reading index/value pairs, hence >>1
		int index = Send_buffer_get(&ring_buffer_CAN_Beacon);
		int value = Send_buffer_get(&ring_buffer_CAN_Beacon);
		Transmit_buffer_put(&TransmitBuffer, whereToSend, index, value);
	}
	if(TxKickNeeded) {
		can_tx_kick(TRANSMITMOB);
	}

}

int GetTransmitErrorCount(void) {
	return TransmitSetMissMatch;
}

//End Transmit Functions

// disassembles the data and places it in a buffer to be sent

void Send_buffer_put(struct ringBufSCAN *_this, unsigned int towhere, unsigned int towhat) {


	if (_this->count < (BUFFER_SIZECAN - 3)) {
		_this->buf[_this->head] = towhere;
		_this->head = Send_buffer_modulo_inc(_this->head, BUFFER_SIZECAN);
		++_this->count;
		_this->buf[_this->head] = towhat;
		_this->head = Send_buffer_modulo_inc(_this->head, BUFFER_SIZECAN);
		++_this->count;
	}

}

void Transmit_buffer_put(struct ringBufTRANSMIT*_this, unsigned int dest, unsigned int index, unsigned int value) {


	if (_this->count < (BUFFER_SIZETRANSMIT - 3)) {
		_this->buf[_this->head] = dest;
		_this->head = Send_buffer_modulo_inc(_this->head, BUFFER_SIZETRANSMIT);
		++_this->count;
		_this->buf[_this->head] = index;
		_this->head = Send_buffer_modulo_inc(_this->head, BUFFER_SIZETRANSMIT);
		++_this->count;
		_this->buf[_this->head] = value;
		_this->head = Send_buffer_modulo_inc(_this->head, BUFFER_SIZETRANSMIT);
		++_this->count;
	}

}


//pulls info out of the send buffer in a first in first out fashion

unsigned int Send_buffer_get(struct ringBufSCAN* _this) {
	unsigned int c;
	if (_this->count > 0) {
		c = _this->buf[_this->tail];
		_this->tail = Send_buffer_modulo_inc(_this->tail, BUFFER_SIZECAN);
		--_this->count;
	} else {
		c = 0;
	}
	return (c);
}

unsigned int Transmit_buffer_get(struct ringBufTRANSMIT* _this) {
	unsigned int c;
	if (_this->count > 0) {
		c = _this->buf[_this->tail];
		_this->tail = Send_buffer_modulo_inc(_this->tail, BUFFER_SIZETRANSMIT);
		--_this->count;
	} else {
		c = 0;
	}
	return (c);
}
unsigned int Transmit_buffer_peek(struct ringBufTRANSMIT* _this) {
	unsigned int c;
	if (_this->count > 0) {
		c = _this->buf[_this->tail];
	}
	else {
		c = 0;
	}
	return (c);
}

void *memset(void *s, int c, size_t n);

//flushes the send buffer to get it ready for new data

void Send_buffer_flush(struct ringBufSCAN* _this, const int clearBuffer) {
	_this->count = 0;
	_this->head = 0;
	_this->tail = 0;
	if (clearBuffer) {
		memset(_this->buf, 0, sizeof (_this->buf));
	}
}

void Transmit_buffer_flush(struct ringBufTRANSMIT* _this, const int clearBuffer) {
	_this->count = 0;
	_this->head = 0;
	_this->tail = 0;
	if (clearBuffer) {
		memset(_this->buf, 0, sizeof (_this->buf));
	}
}


// increments counters for the buffer functions

unsigned int Send_buffer_modulo_inc(const unsigned int value, const unsigned int modulus) {
	unsigned int my_value = value + 1;
	if (my_value >= modulus) {
		my_value = 0;
	}
	return (my_value);
}

//getter for send circular buffer.
unsigned int Send_buffer_GetCount(struct ringBufSCAN* _this) {
	return _this->count;

}

unsigned int Transmit_buffer_GetCount(struct ringBufTRANSMIT* _this) {
	return _this->count;

}