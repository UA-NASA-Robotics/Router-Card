/*
 * FastTransfer.c
 *
 * Created: 10/19/2015 11:32:29 AM
 *  Author: DFT
 */ 



#include <stdbool.h>
#include <stdlib.h>
#include <avr/io.h>
#include "FastTransfer.h"
<<<<<<< HEAD
//#include "..\..\..\tracee\Common\CommsDefenition.h"
#include "Definitions.h"
=======
#include "..\..\..\Common\CommsDefenition.h"
>>>>>>> develop
#include "AT90CAN_UART.h"
#include "CANFastTransfer.h"
#include "Config.h"
#include "LEDs.h"
//#include "CogniPins.h"

#define MotorControllerAddress 6

int Receive0[50];
int Receive1[50];


int * getReceiveArray1(void)
{
	return Receive1;
}
int * getReceiveArray0(void)
{
	return Receive0;
}

void initFastTransfer(void)
{
	begin0(Receive0, sizeof(Receive0), MotorControllerAddress, 0, &USART0_put_C, &USART0_get_C, &USART0_Available, &USART0_peek_C);
	begin1(Receive1, sizeof(Receive1), MotorControllerAddress, 0, &USART1_put_C, &USART1_get_C, &USART1_Available, &USART1_peek_C);
}
//Captures address of receive array, the max data address, the address of the module, true/false if AKNAKs are wanted and the Serial address

void wipeRxBuffer0(void)
{
	int i=0;
	for(i=0;i<RX_BUFFER_SIZE0;i++)
	{
		rx_buffer0[i]=0;
	}
}

void wipeRxBuffer1(void)
{
	int i=0;
	for(i=0;i<RX_BUFFER_SIZE1;i++)
	{
		rx_buffer1[i]=0;
	}
}

//void begin(volatile int * ptr, unsigned char maxSize, unsigned char givenAddress, bool error, void (*stufftosend)(unsigned char), unsigned char (*stufftoreceive)(void), int (*stuffavailable)(void), unsigned char (*stuffpeek)(void)) {
void begin0(volatile int * ptr, unsigned char maxSize, unsigned char givenAddress, bool error, void (*stufftosend)(unsigned char), unsigned char (*stufftoreceive)(void), int (*stuffavailable)(void), unsigned char (*stuffpeek)(void)) {
	receiveArrayAddress0 = ptr;
	moduleAddress0 = givenAddress;
	serial_write0 = stufftosend;
	serial_available0 = stuffavailable;
	serial_peek0 = stuffpeek;
	serial_read0 = stufftoreceive;
	maxDataAddress0 = maxSize / 4;
	sendStructAddress0 = ( unsigned char*) & ring_buffer0;
	AKNAKsend0 = error;
	alignErrorCounter0 = 0;

}
void begin1(volatile int * ptr, unsigned char maxSize, unsigned char givenAddress, bool error, void (*stufftosend)(unsigned char), unsigned char (*stufftoreceive)(void), int (*stuffavailable)(void), unsigned char (*stuffpeek)(void)) {
	receiveArrayAddress1 = ptr;
	moduleAddress1 = givenAddress;
	serial_write1 = stufftosend;
	serial_available1 = stuffavailable;
	serial_peek1 = stuffpeek;
	serial_read1 = stufftoreceive;
	maxDataAddress1 = maxSize / 4;
	sendStructAddress1 = ( unsigned char*) & ring_buffer1;
	AKNAKsend1 = error;
	alignErrorCounter1 = 0;

}

//CRC Calculator

unsigned char CRC8(const unsigned char * data, unsigned char len) {
	unsigned char crc = 0x00;
	while (len--) {
		unsigned char extract = *data++;
		unsigned char tempI;
		for (tempI = 8; tempI; tempI--) {
			unsigned char sum = (crc ^ extract) & 0x01;
			crc >>= 1;
			if (sum) {
				crc ^= polynomial;
			}
			extract >>= 1;
		}
	}
	return crc;
}

//Sends out send buffer with a 2 start bytes, where the packet is going, where it came from, the size of the data packet, the data and the crc.
void sendData0(unsigned char whereToSend) {

	//calculate the crc
	unsigned char CS = CRC8(sendStructAddress0, ring_buffer0.count);

	serial_write0(0x06); //start address
	serial_write0(0x85); //start address
	serial_write0(whereToSend);
	serial_write0(moduleAddress0);
	serial_write0(ring_buffer0.count); //length of packet not including the crc


	//send the rest of the packet
	int i;
	for (i = 0; i < ring_buffer0.count; i++) {
		serial_write0(*(sendStructAddress0 + i));
	}

	//send the crc
	serial_write0(CS);

	//record the sent message data for aknak check later
	crcBufS_put(&crc_buffer0, whereToSend, CS, 0);

	// clears the buffer after a sending
	FastTransfer_buffer_flush0(&ring_buffer0, 1);

}
void sendData1(unsigned char whereToSend) {

	//calculate the crc
	unsigned char CS = CRC8(sendStructAddress1, ring_buffer1.count);

	serial_write1(0x06); //start address
	serial_write1(0x85); //start address
	serial_write1(whereToSend);
	serial_write1(moduleAddress1);
	serial_write1(ring_buffer1.count); //length of packet not including the crc


	//send the rest of the packet
	int i;
	for (i = 0; i < ring_buffer1.count; i++) {
		serial_write1(*(sendStructAddress1 + i));
	}

	//send the crc
	serial_write1(CS);

	//record the sent message data for aknak check later
	crcBufS_put(&crc_buffer1, whereToSend, CS, 0);

	// clears the buffer after a sending
	FastTransfer_buffer_flush1(&ring_buffer1, 0);

}

bool receiveData0(void) {
	
	//start off by looking for the header bytes. If they were already found in a previous call, skip it.
	if (rx_len0 == 0) {
		
		//this size check may be redundant due to the size check below, but for now I'll leave it the way it is.
		if (serial_available0() > 4) {
			
			//this will block until a 0x06 is found or buffer size becomes less then 3.
			while (serial_read0() != 0x06) {
				//This will trash any preamble junk in the serial buffer
				//but we need to make sure there is enough in the buffer to process while we trash the rest
				//if the buffer becomes too empty, we will escape and try again on the next call
				alignErrorCounter0++; //increments the counter whenever a byte is trashed
				if (serial_available0() < 5)
				return false;
			}
			if (serial_read0() == 0x85) {
				rx_address0 = serial_read0(); // pulls the address
				returnAddress0 = serial_read0(); // pulls where the message came from
				rx_len0 = serial_read0(); // pulls the length
				//make sure the address received is a match for this module if not throw the packet away
				//if (rx_address != moduleAddress) {
				if (rx_address0 != moduleAddress0 && (rx_address0<RouterCardAddress || rx_address0>BucketAddress)) {
					addressErrorCounter0++; // increments a counter whenever the wrong address is received
					//if the address does not match the buffer is flushed for the size of
					//the data packet plus one for the CRC
					//int u;
					//for (u = 0; u <= (rx_len + 1); u++) {
					//serial_read();
					//}
					
					rx_len0 = 0; // reset length
					return false;
				}
				// if the address matches the a dynamic buffer is created to store the received data
				//rx_buffer = (unsigned char*) malloc(rx_len + 1);
			}
		}
	}

	//we get here if we already found the header bytes, the address matched what we know, and now we are byte aligned.
	if (rx_len0 != 0) {

		//this check is preformed to see if the first data address is a 255, if it is then this packet is an AKNAK
		if (rx_array_inx0 == 0) {
			while (!(serial_available0() >= 1));
			if (255 == serial_peek0()) {
				CRCcheck();
				rx_len0 = 0;
				rx_array_inx0 = 0;
				wipeRxBuffer0();
				//free(rx_buffer);
				return receiveData0();
			}
		}


		while (serial_available0() && rx_array_inx0 <= rx_len0) {
			rx_buffer0[rx_array_inx0++] = serial_read0();
		}

		if (rx_len0 == (rx_array_inx0 - 1)) {
			//seem to have got whole message
			//last uint8_t is CS
			calc_CS0 = CRC8(rx_buffer0, rx_len0);



			if (calc_CS0 == rx_buffer0[rx_array_inx0 - 1]) {//CS good

				// reassembles the data and places it into the receive array according to data address.
				int r;
				
				if( rx_address0 == moduleAddress0) {
					for (r = 0; r < rx_len0; r = r + 3) {
						if (rx_buffer0[r] < maxDataAddress0) {
							group.parts[0] = rx_buffer0[r + 1];
							group.parts[1] = rx_buffer0[r + 2];
							receiveArrayAddress0[(rx_buffer0[r])] = group.integer;
							} else {
							dataAdressErrorCounter0++;
						}
					}
				}
				
				#ifndef DISABLE_CAN_FORWARDING
				else { // must be going to CAN bus
					for (r = 0; r < rx_len0; r = r + 3) {
						//if (rx_buffer[r] < maxDataAddressNavigation) { no protection against bad address here
						//all protection is handled by the processor receiving packets.
						//if bus is being populated by bad packets a protection here will help keep the bus less busy
						group.parts[0] = rx_buffer0[r + 1];
						group.parts[1] = rx_buffer0[r + 2];
						ToSendCAN_Beacon((unsigned int)rx_buffer0[r], group.integer);
						//toggelLED(8);
						//} else {
						//dataAdressErrorCounter++;
						//}
					}
					sendDataCAN_Beacon(rx_address0);
				}
				#endif


				if (AKNAKsend0) { // if enabled sends an AK
					unsigned char holder[3];
					holder[0] = 255;
					holder[1] = 1;
					holder[2] = rx_buffer0[rx_array_inx0 - 1];
					unsigned char crcHolder = CRC8(holder, 3);
					serial_write0(0x06);
					serial_write0(0x85);
					serial_write0(returnAddress0);
					serial_write0(moduleAddress0);
					serial_write0(3);
					serial_write0(255);
					serial_write0(1);
					serial_write0(rx_buffer0[rx_array_inx0 - 1]);
					serial_write0(crcHolder);
				}



				rx_len0 = 0;
				rx_array_inx0 = 0;
				wipeRxBuffer0();
				return true;
				} else {
				crcErrorCounter0++; //increments the counter every time a crc fails

				if (AKNAKsend0) { // if enabled sends NAK
					unsigned char holder[3];
					holder[0] = 255;
					holder[1] = 2;
					holder[2] = rx_buffer0[rx_array_inx0 - 1];
					unsigned char crcHolder = CRC8(holder, 3);
					serial_write0(0x06);
					serial_write0(0x85);
					serial_write0(returnAddress0);
					serial_write0(moduleAddress0);
					serial_write0(3);
					serial_write0(255);
					serial_write0(2);
					serial_write0(rx_buffer0[rx_array_inx0 - 1]);
					serial_write0(crcHolder);
				}

				//failed checksum, need to clear this out
				rx_len0 = 0;
				rx_array_inx0 = 0;
				wipeRxBuffer0();
				return false;
			}
		}
	}


	return false;
}
bool receiveData1(void) 
{
	//start off by looking for the header bytes. If they were already found in a previous call, skip it.
	if (rx_len1 == 0) 
	{
		//this size check may be redundant due to the size check below, but for now I'll leave it the way it is.
		if (serial_available1() > 4) 
		{
			//this will block until a 0x06 is found or buffer size becomes less then 3.
			while (serial_peek1() != 0x06) 
			{
				serial_read1();
				//This will trash any preamble junk in the serial buffer
				//but we need to make sure there is enough in the buffer to process while we trash the rest
				//if the buffer becomes too empty, we will escape and try again on the next call
				alignErrorCounter1++; //increments the counter whenever a byte is trashed
				if (serial_available1() < 5)
				return false;
			}
			if(serial_peek1()==0x06)
			{
				serial_read1();
			}
			else
			{
				return false;
			}
			if (serial_read1() == 0x85) 
			{
				rx_address1 = serial_read1(); // pulls the address
				returnAddress1 = serial_read1(); // pulls where the message came from
				rx_len1 = serial_read1(); // pulls the length
				//make sure the address received is a match for this module if not throw the packet away
				//if (rx_address != moduleAddress) {
				if (rx_address1 != moduleAddress1 && (rx_address1<=RouterCardAddress || rx_address1>=BucketAddress)) {
					addressErrorCounter1++; // increments a counter whenever the wrong address is received
					//if the address does not match the buffer is flushed for the size of
					//the data packet plus one for the CRC
					//int u;
					//for (u = 0; u <= (rx_len + 1); u++) {
						//serial_read();
					//}
					
					rx_len1 = 0; // reset length
					return false;
				}
				// if the address matches the a dynamic buffer is created to store the received data
				//rx_buffer = (unsigned char*) malloc(rx_len + 1);
			}
		}
	}

	//we get here if we already found the header bytes, the address matched what we know, and now we are byte aligned.
	if (rx_len1 != 0) 
	{
		//this check is preformed to see if the first data address is a 255, if it is then this packet is an AKNAK
		if (rx_array_inx1 == 0) 
		{
			while (!(serial_available1() >= 1));
			if (255 == serial_peek1()) 
			{
				CRCcheck();
				rx_len1 = 0;
				rx_array_inx1 = 0;
				wipeRxBuffer1();
				return receiveData1();
			}
		}


		while (serial_available1() && rx_array_inx1 <= rx_len1) 
		{
			rx_buffer1[rx_array_inx1++] = serial_read1();
		}

		if (rx_len1 == (rx_array_inx1 - 1)) 
		{
			//seem to have got whole message
			//last uint8_t is CS
			calc_CS1 = CRC8(rx_buffer1, rx_len1);

			if (calc_CS1 == rx_buffer1[rx_array_inx1 - 1]) //CS good
			{

				// reassembles the data and places it into the receive array according to data address.
				int r;
				if( rx_address1 == moduleAddress1) 
				{
					for (r = 0; r < rx_len1; r = r + 3) 
					{
						if (rx_buffer1[r] < maxDataAddress1) 
						{
							group.parts[0] = rx_buffer1[r + 1];
							group.parts[1] = rx_buffer1[r + 2];
							receiveArrayAddress1[(rx_buffer1[r])] = group.integer;
						}
						else
						{
							dataAdressErrorCounter1++;
						}
					}
				}
				#ifndef DISABLE_CAN_FORWARDING
				else { // must be going to CAN bus
					setLED(LED7, ON); 
					for (r = 0; r < rx_len1; r = r + 3) 
					{
						//if (rx_buffer[r] < maxDataAddressNavigation) { no protection against bad address here
							//all protection is handled by the processor receiving packets. 
							//if bus is being populated by bad packets a protection here will help keep the bus less busy
							group.parts[0] = rx_buffer1[r + 1];
							group.parts[1] = rx_buffer1[r + 2];
							ToSendCAN_Control((unsigned int)rx_buffer1[r], group.integer);
							//} else {
							//dataAdressErrorCounter++;
						//}
					}
					sendDataCAN_Control(rx_address1); 
				}
				#endif


				if (AKNAKsend1) // if enabled sends an AK
				{ 
					unsigned char holder[3];
					holder[0] = 255;
					holder[1] = 1;
					holder[2] = rx_buffer1[rx_array_inx1 - 1];
					unsigned char crcHolder = CRC8(holder, 3);
					serial_write1(0x06);
					serial_write1(0x85);
					serial_write1(returnAddress1);
					serial_write1(moduleAddress1);
					serial_write1(3);
					serial_write1(255);
					serial_write1(1);
					serial_write1(rx_buffer1[rx_array_inx1 - 1]);
					serial_write1(crcHolder);
				}
				rx_len1 = 0;
				rx_array_inx1 = 0;
				wipeRxBuffer1();
				return true;
			} 
			else 
			{
				crcErrorCounter1++; //increments the counter every time a crc fails

				if (AKNAKsend1) // if enabled sends NAK
				{ 
					unsigned char holder[3];
					holder[0] = 255;
					holder[1] = 2;
					holder[2] = rx_buffer1[rx_array_inx1 - 1];
					unsigned char crcHolder = CRC8(holder, 3);
					serial_write1(0x06);
					serial_write1(0x85);
					serial_write1(returnAddress1);
					serial_write1(moduleAddress1);
					serial_write1(3);
					serial_write1(255);
					serial_write1(2);
					serial_write1(rx_buffer1[rx_array_inx1 - 1]);
					serial_write1(crcHolder);
				}

				//failed checksum, need to clear this out
				rx_len1 = 0;
				rx_array_inx1 = 0;
				wipeRxBuffer1();
				return false;
			}
		}
	}
	return false;
}


// populates what info needs sent and to what data address
void ToSend0(unsigned char where, unsigned int what) {
	FastTransfer_buffer_put0(&ring_buffer0, where, what);
}
void ToSend1(unsigned char where, unsigned int what) {
	FastTransfer_buffer_put1(&ring_buffer1, where, what);
}


// disassembles the data and places it in a buffer to be sent
void FastTransfer_buffer_put0(struct ringBufS0 *_this, unsigned char towhere, unsigned int towhat) {

	group.integer = towhat;

	if (_this->count < (BUFFER_SIZE0 - 3)) {
		_this->buf[_this->head] = towhere;
		_this->head = FastTransfer_buffer_modulo_inc(_this->head, BUFFER_SIZE0);
		++_this->count;
		_this->buf[_this->head] = group.parts[0];
		_this->head = FastTransfer_buffer_modulo_inc(_this->head, BUFFER_SIZE0);
		++_this->count;
		_this->buf[_this->head] = group.parts[1];
		_this->head = FastTransfer_buffer_modulo_inc(_this->head, BUFFER_SIZE0);
		++_this->count;

	}

}
void FastTransfer_buffer_put1(struct ringBufS1 *_this, unsigned char towhere, unsigned int towhat) {

	group.integer = towhat;

	if (_this->count < (BUFFER_SIZE1 - 3)) {
		_this->buf[_this->head] = towhere;
		_this->head = FastTransfer_buffer_modulo_inc(_this->head, BUFFER_SIZE1);
		++_this->count;
		_this->buf[_this->head] = group.parts[0];
		_this->head = FastTransfer_buffer_modulo_inc(_this->head, BUFFER_SIZE1);
		++_this->count;
		_this->buf[_this->head] = group.parts[1];
		_this->head = FastTransfer_buffer_modulo_inc(_this->head, BUFFER_SIZE1);
		++_this->count;

	}

}


//pulls info out of the send buffer in a first in first out fashion
unsigned char FastTransfer_buffer_get0(struct ringBufS0* _this) {
	unsigned char c;
	if (_this->count > 0) {
		c = _this->buf[_this->tail];
		_this->tail = FastTransfer_buffer_modulo_inc(_this->tail, BUFFER_SIZE0);
		--_this->count;
		} else {
		c = 0;
	}
	return (c);
}
unsigned char FastTransfer_buffer_get1(struct ringBufS1* _this) {
	unsigned char c;
	if (_this->count > 0) {
		c = _this->buf[_this->tail];
		_this->tail = FastTransfer_buffer_modulo_inc(_this->tail, BUFFER_SIZE1);
		--_this->count;
		} else {
		c = 0;
	}
	return (c);
}

//void *memset(void *s, int c, size_t n);

//flushes the send buffer to get it ready for new data
void FastTransfer_buffer_flush0(struct ringBufS0* _this, const int clearBuffer) {
	_this->count = 0;
	_this->head = 0;
	_this->tail = 0;
	if (clearBuffer) {
		//memset(_this->buf, 0, sizeof (_this->buf));
	}
}
void FastTransfer_buffer_flush1(struct ringBufS1* _this, const int clearBuffer) {
	_this->count = 0;
	_this->head = 0;
	_this->tail = 0;
	if (clearBuffer) {
		//memset(_this->buf, 0, sizeof (_this->buf));
	}
}


// increments counters for the buffer functions

unsigned int FastTransfer_buffer_modulo_inc(const unsigned int value, const unsigned int modulus) {
	unsigned int my_value = value + 1;
	if (my_value >= modulus) {
		my_value = 0;
	}
	return (my_value);
}


//searches the buffer for the status of a message that was sent

unsigned char AKNAK1(unsigned char module) {
	int r;
	for (r = 0; r < CRC_COUNT; r++) {
		if (module == crcBufS_get(&crc_buffer1, r, 0)) {
			return crcBufS_get(&crc_buffer1, r, 2);
		}
	}
	return 4;
}


//returns align error
unsigned int alignError0(void) {
	return alignErrorCounter0;
}
unsigned int alignError1(void) {
	return alignErrorCounter1;
}


//returns CRC error
unsigned int CRCError0(void) {
	return crcErrorCounter0;
}
unsigned int CRCError1(void) {
	return crcErrorCounter1;
}


//returns address error
unsigned int addressError10(void) {
	return addressErrorCounter0;
}
unsigned int addressError1(void) {
	return addressErrorCounter1;
}

unsigned int dataAddressError0(void) {
	return dataAdressErrorCounter0;
}
unsigned int dataAddressError1(void) {
	return dataAdressErrorCounter1;
}

// after a packet is sent records the info of that packet

void crcBufS_put(struct crcBufS* _this, unsigned char address, unsigned char oldCRC, unsigned char status) {
	_this->buf[_this->head] = address;
	_this->head++;
	_this->buf[_this->head] = oldCRC;
	_this->head++;
	_this->buf[_this->head] = status;
	_this->head++;
	if (_this->head >= CRC_BUFFER_SIZE) {
		_this->head = 0;
	}
}


// after a Ak or NAK is received that status is stored

void crcBufS_status_put(struct crcBufS* _this, unsigned char time, unsigned char status) {
	if (time >= CRC_COUNT) {
		time = CRC_COUNT - 1;
	}
	time = time + 1;
	int wantedTime = time * 3;
	if (wantedTime > _this->head) {
		wantedTime = (CRC_BUFFER_SIZE) - (wantedTime - _this->head);
		_this->buf[(wantedTime + 2)] = status;
		} else {
		_this->buf[(_this->head - wantedTime) + 2] = status;
	}
}


// pulls data from the AKNAK buffer

unsigned char crcBufS_get(struct crcBufS* _this, unsigned char time, unsigned char space) {
	if (time >= CRC_COUNT) {
		time = CRC_COUNT - 1;
	}
	if (space >= CRC_DEPTH) {
		space = CRC_DEPTH - 1;
	}
	time = time + 1;
	int wantedTime = time * 3;
	if (wantedTime > _this->head) {
		wantedTime = (CRC_BUFFER_SIZE) - (wantedTime - _this->head);
		return (_this->buf[(wantedTime + space)]);
		} else {
		return (_this->buf[(_this->head - wantedTime) + space]);
	}
}


//when an AK or NAK is received this compares it to the buffer and records the status

void CRCcheck(void) {

	while (!(serial_available1() > 3)); // trap makes sure that there are enough bytes in the buffer for the AKNAK check

	unsigned char arrayHolder[3];
	arrayHolder[0] = serial_read1();
	arrayHolder[1] = serial_read1();
	arrayHolder[2] = serial_read1();
	unsigned char SentCRC = serial_read1();
	unsigned char calculatedCRC = CRC8(arrayHolder, 3);


	if (SentCRC == calculatedCRC) {

		int rt;
		for (rt = 0; rt < CRC_COUNT; rt++) {
			if (returnAddress1 == crcBufS_get(&crc_buffer1, rt, 0)) {
				if (arrayHolder[2] == crcBufS_get(&crc_buffer1, rt, 1)) {
					if (arrayHolder[1] == 1) {
						crcBufS_status_put(&crc_buffer1, rt, 1);
						break;
						} else if (arrayHolder[1] == 2) {
						crcBufS_status_put(&crc_buffer1, rt, 2);
						break;
					}
				}
			}
		}
		} else {
		crcErrorCounter1++;
	} //increments the counter every time a crc fails
}

