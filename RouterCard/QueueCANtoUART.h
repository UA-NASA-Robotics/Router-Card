/*
 * QueueCANtoUART.h
 *
 * Created: 3/15/2017 9:43:23 PM
 *  Author: Reed
 */ 


#ifndef QUEUECANTOUART_H_
#define QUEUECANTOUART_H_

#define BUFFER_SIZE_QUEUE 30 //should be a multiple of 3 for best use of memory

char Send_Queue_ControlBox_put(char from, unsigned char where, unsigned int what);
char Send_Queue_Beacon_put(char from, unsigned char where, unsigned int what); 

char Send_Queue_ControlBox_send(char from);
char Send_Queue_Beacon_send(char from);

struct ringBufQueue { // this is where the send data is stored before sending
	unsigned char buf[BUFFER_SIZE_QUEUE];
	int head;
	int tail;
	int count;
};


//union stuff { // this union is used to join and disassemble integers
	//unsigned char parts[2];
	//unsigned int integer;
//};
//union stuff group;
void Queue_buffer_put(struct ringBufQueue *_this, const unsigned char towhere, const unsigned int towhat);
unsigned char Queue_buffer_get(struct ringBufQueue* _this);
void Queue_buffer_flush(struct ringBufQueue* _this, const int clearBuffer);
unsigned int Queue_buffer_modulo_inc(const unsigned int value, const unsigned int modulus);

#endif /* QUEUECANTOUART_H_ */