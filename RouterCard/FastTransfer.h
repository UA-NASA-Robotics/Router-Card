/*
 * FastTransfer.h
 *
 * Created: 10/19/2015 11:32:39 AM
 *  Author: DFT
 */ 


#ifndef FASTTRANSFER_H_
#define FASTTRANSFER_H_

#define RX_BUFFER_SIZE0 250
#define RX_BUFFER_SIZE1 250
//the capital D is so there is no interference with the lower case d of EasyTransfer
#define Details(name) (int*)&name,sizeof(name)


int * getReceiveArray0(void);
int * getReceiveArray1(void);
void initFastTransfer(void);
//void begin(volatile int * ptr, unsigned char maxSize, unsigned char givenAddress, bool error, void (*stufftosend)(unsigned char), unsigned char (*stufftoreceive)(void), int (*stuffavailable)(void), unsigned char (*stuffpeek)(void));
void begin0(volatile int * ptr, unsigned char maxSize, unsigned char givenAddress, bool error, void (*stufftosend)(unsigned char), unsigned char (*stufftoreceive)(void), int (*stuffavailable)(void), unsigned char (*stuffpeek)(void));
void begin1(volatile int * ptr, unsigned char maxSize, unsigned char givenAddress, bool error, void (*stufftosend)(unsigned char), unsigned char (*stufftoreceive)(void), int (*stuffavailable)(void), unsigned char (*stuffpeek)(void));
void sendData0(unsigned char whereToSend);
void sendData1(unsigned char whereToSend);
bool receiveData0(void);
bool receiveData1(void);
void ToSend0(const unsigned char where, const unsigned int what);
void ToSend1(const unsigned char where, const unsigned int what);
unsigned char AKNAK0(unsigned char module);
unsigned char AKNAK1(unsigned char module);
unsigned int alignError0(void);
unsigned int alignError1(void);
unsigned int CRCError0(void);
unsigned int CRCError1(void);
unsigned int addressError0(void);
unsigned int addressError1(void);
unsigned int dataAddressError0(void);
unsigned int dataAddressError1(void);

volatile int receiveArray0[50];
volatile int receiveArray1[50];

void (*serial_write0)(unsigned char);
void (*serial_write1)(unsigned char);
unsigned char (*serial_read0)(void);
unsigned char (*serial_read1)(void);
int (*serial_available0)(void);
int (*serial_available1)(void);
unsigned char (*serial_peek0)(void);
unsigned char (*serial_peek1)(void);
unsigned char rx_buffer0[RX_BUFFER_SIZE0]; //address for temporary storage and parsing buffer
unsigned char rx_buffer1[RX_BUFFER_SIZE1]; //address for temporary storage and parsing buffer
unsigned char rx_array_inx0; //index for RX parsing buffer
unsigned char rx_array_inx1; //index for RX parsing buffer
unsigned char rx_len0; //RX packet length according to the packet
unsigned char rx_len1; //RX packet length according to the packet
unsigned char calc_CS0; //calculated Checksum
unsigned char calc_CS1; //calculated Checksum
unsigned char moduleAddress0; // the address of this module
unsigned char moduleAddress1; // the address of this module
unsigned char returnAddress0; //the address to send the crc back to
unsigned char returnAddress1; //the address to send the crc back to
unsigned char maxDataAddress0; //max address allowable
unsigned char maxDataAddress1; //max address allowable
volatile int * receiveArrayAddress0; // this is where the data will go when it is received
volatile int * receiveArrayAddress1; // this is where the data will go when it is received
unsigned char * sendStructAddress0; // this is where the data will be sent from
unsigned char * sendStructAddress1; // this is where the data will be sent from
bool AKNAKsend0; // turns the acknowledged or not acknowledged on/off
bool AKNAKsend1; // turns the acknowledged or not acknowledged on/off
unsigned int alignErrorCounter0; //counts the align errors
unsigned int alignErrorCounter1; //counts the align errors
unsigned int crcErrorCounter0; // counts any failed crcs
unsigned int crcErrorCounter1; // counts any failed crcs
unsigned int addressErrorCounter0; // counts every time a wrong address is received
unsigned int addressErrorCounter1; // counts every time a wrong address is received
unsigned int dataAdressErrorCounter0; // counts if the received data fall outside of the receive array
unsigned int dataAdressErrorCounter1; // counts if the received data fall outside of the receive array
unsigned char rx_address0; //RX address received
unsigned char rx_address1; //RX address received

#define polynomial 0x8C  //polynomial used to calculate crc
#define BUFFER_SIZE0 200 //ring buffer size
#define BUFFER_SIZE1 200 //ring buffer size
#define CRC_COUNT 5 // how many AKNAKs are stored
#define CRC_DEPTH 3  // how many pieces of data are stored with each CRC send event
#define CRC_BUFFER_SIZE (CRC_COUNT * CRC_DEPTH) //crc buffer size 5 deep and 3 bytes an entry

struct ringBufS0 { // this is where the send data is stored before sending
	unsigned char buf[BUFFER_SIZE0];
	int head;
	int tail;
	int count;
};
struct ringBufS0 ring_buffer0;

struct ringBufS1 { // this is where the send data is stored before sending
	unsigned char buf[BUFFER_SIZE1];
	int head;
	int tail;
	int count;
};
struct ringBufS1 ring_buffer1;

union stuff { // this union is used to join and disassemble integers
	unsigned char parts[2];
	unsigned int integer;
}stuff;
union stuff group;

struct crcBufS { // this is where the address where sent to, the sent crc, the status of the AKNAK
	unsigned char buf[CRC_BUFFER_SIZE];
	int head;
};
struct crcBufS crc_buffer0;
struct crcBufS crc_buffer1;

unsigned char CRC8(const unsigned char * data, unsigned char len);
void FastTransfer_buffer_put0(struct ringBufS0 *_this, const unsigned char towhere, const unsigned int towhat);
void FastTransfer_buffer_put1(struct ringBufS1 *_this, const unsigned char towhere, const unsigned int towhat);
unsigned char FastTransfer_buffer_get0(struct ringBufS0* _this);
unsigned char FastTransfer_buffer_get1(struct ringBufS1* _this);
void FastTransfer_buffer_flush0(struct ringBufS0* _this, const int clearBuffer);
void FastTransfer_buffer_flush1(struct ringBufS1* _this, const int clearBuffer);
unsigned int FastTransfer_buffer_modulo_inc(const unsigned int value, const unsigned int modulus);
void crcBufS_put(struct crcBufS* _this, unsigned char address, unsigned char oldCRC, unsigned char status);
void crcBufS_status_put(struct crcBufS* _this, unsigned char time, unsigned char status);
unsigned char crcBufS_get(struct crcBufS* _this, unsigned char time, unsigned char space);
void CRCcheck(void);







#endif /* FASTTRANSFER_H_ */