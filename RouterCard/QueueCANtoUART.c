/*
 * QueueCANtoUART.c
 *
 * Created: 3/15/2017 9:43:00 PM
 *  Author: Reed
 */ 
#include <stdbool.h>
#include "QueueCANtoUART.h"

#include "CommsDefenition.h"

#include "FastTransfer.h"

#include <stdlib.h>

struct ringBufQueue ControlBox_Buffer_To_Beacon;
struct ringBufQueue Debugger_Buffer_To_Beacon;
struct ringBufQueue Router_Buffer_To_Beacon;
struct ringBufQueue Navigation_Buffer_To_Beacon;
struct ringBufQueue Sensor_Buffer_To_Beacon;
struct ringBufQueue MouseGyro_Buffer_To_Beacon;
struct ringBufQueue Bucket_Buffer_To_Beacon;

struct ringBufQueue Beacon_Buffer_To_ControlBox;
struct ringBufQueue Debugger_Buffer_To_ControlBox;
struct ringBufQueue Router_Buffer_To_ControlBox;
struct ringBufQueue Navigation_Buffer_To_ControlBox;
struct ringBufQueue Sensor_Buffer_To_ControlBox;
struct ringBufQueue MouseGyro_Buffer_To_ControlBox;
struct ringBufQueue Bucket_Buffer_To_ControlBox;

char Send_Queue_ControlBox_put(char from, unsigned char where, unsigned int what){

	switch(from) {
		case BeaconAddress:
			Queue_buffer_put(&Beacon_Buffer_To_ControlBox, where, what);
		break;
		
		case DebuggerAddress:
			Queue_buffer_put(&Debugger_Buffer_To_ControlBox, where, what);
		break;
		
		case RouterCardAddress:
			Queue_buffer_put(&Router_Buffer_To_ControlBox, where, what);
		break;
		
		//case NavigationAddress:
			//Queue_buffer_put(&Navigation_Buffer_To_ControlBox, where, what);
		//break;
		//
		//case SensorAddress:
			//Queue_buffer_put(&Sensor_Buffer_To_ControlBox, where, what);
		//break;
		//
		//case MouseGyroAddress:
			//Queue_buffer_put(&MouseGyro_Buffer_To_ControlBox, where, what);
		//break;
		//
		case BucketAddress:
			Queue_buffer_put(&Bucket_Buffer_To_ControlBox, where, what);
		break;
		
		default : /* Optional */
		return 0;
	}
	return 1;
	
}
char Send_Queue_Beacon_put(char from, unsigned char where, unsigned int what){

	switch(from) {
		case ControlBoxAddress:
			Queue_buffer_put(&ControlBox_Buffer_To_Beacon, where, what);
		break;
		
		case DebuggerAddress:
			Queue_buffer_put(&Debugger_Buffer_To_Beacon, where, what);
		break;
		
		case RouterCardAddress:
			Queue_buffer_put(&Router_Buffer_To_Beacon, where, what);
		break;
		
		case NavigationAddress:
			Queue_buffer_put(&Navigation_Buffer_To_Beacon, where, what);
		break;
		
		case SensorAddress:
			Queue_buffer_put(&Sensor_Buffer_To_Beacon, where, what);
		break;
		
		case MouseGyroAddress:
			Queue_buffer_put(&MouseGyro_Buffer_To_Beacon, where, what);
		break;
		
		case BucketAddress:
			Queue_buffer_put(&Bucket_Buffer_To_Beacon, where, what);
		break;
		
		default : /* Optional */
		return 0; 
	}
	return 1; 
	
}

char Send_Queue_ControlBox_send(char from){
	unsigned char where;
	switch(from) {
		case BeaconAddress:
			for (int i = 0; i < Beacon_Buffer_To_ControlBox.count; i = i+3) {
				where = Queue_buffer_get(&Beacon_Buffer_To_ControlBox); 
				group.parts[0] = Queue_buffer_get(&Beacon_Buffer_To_ControlBox);
				group.parts[1] = Queue_buffer_get(&Beacon_Buffer_To_ControlBox);
				ToSend1(where,group.integer);
			}
			sendData1(ControlBoxAddress);
		break;
		
		case DebuggerAddress:
			for (int i = 0; i < Debugger_Buffer_To_ControlBox.count; i = i+3) {
				where = Queue_buffer_get(&Debugger_Buffer_To_ControlBox);
				group.parts[0] = Queue_buffer_get(&Debugger_Buffer_To_ControlBox);
				group.parts[1] = Queue_buffer_get(&Debugger_Buffer_To_ControlBox);
				ToSend1(where,group.integer);
			}
			sendData1(ControlBoxAddress);
		break;
		
		case RouterCardAddress:
			for (int i = 0; i < Router_Buffer_To_ControlBox.count; i = i+3) {
				where = Queue_buffer_get(&Router_Buffer_To_ControlBox);
				group.parts[0] = Queue_buffer_get(&Router_Buffer_To_ControlBox);
				group.parts[1] = Queue_buffer_get(&Router_Buffer_To_ControlBox);
				ToSend1(where,group.integer);
			}
			sendData1(ControlBoxAddress);
		break;
		
		case NavigationAddress:
			for (int i = 0; i < Navigation_Buffer_To_ControlBox.count; i = i+3) {
				where = Queue_buffer_get(&Navigation_Buffer_To_ControlBox);
				group.parts[0] = Queue_buffer_get(&Navigation_Buffer_To_ControlBox);
				group.parts[1] = Queue_buffer_get(&Navigation_Buffer_To_ControlBox);
				ToSend1(where,group.integer);
			}
			sendData1(ControlBoxAddress);
		break;
		
		case SensorAddress:
			for (int i = 0; i < Sensor_Buffer_To_ControlBox.count; i = i+3) {
				where = Queue_buffer_get(&Sensor_Buffer_To_ControlBox);
				group.parts[0] = Queue_buffer_get(&Sensor_Buffer_To_ControlBox);
				group.parts[1] = Queue_buffer_get(&Sensor_Buffer_To_ControlBox);
				ToSend1(where,group.integer);
			}
			sendData1(ControlBoxAddress);
		break;
		
		case MouseGyroAddress:
			for (int i = 0; i < MouseGyro_Buffer_To_ControlBox.count; i = i+3) {
				where = Queue_buffer_get(&MouseGyro_Buffer_To_ControlBox);
				group.parts[0] = Queue_buffer_get(&MouseGyro_Buffer_To_ControlBox);
				group.parts[1] = Queue_buffer_get(&MouseGyro_Buffer_To_ControlBox);
				ToSend1(where,group.integer);
			}
			sendData1(ControlBoxAddress);
		break;
		
		case BucketAddress:
			for (int i = 0; i < Bucket_Buffer_To_ControlBox.count; i = i+3) {
				where = Queue_buffer_get(&Bucket_Buffer_To_ControlBox);
				group.parts[0] = Queue_buffer_get(&Bucket_Buffer_To_ControlBox);
				group.parts[1] = Queue_buffer_get(&Bucket_Buffer_To_ControlBox);
				ToSend1(where,group.integer);
			}
			sendData1(ControlBoxAddress);
		break;
		
		/* you can have any number of case statements */
		default : /* Optional */
		return 0;
	}
	return 1;
}

void Queue_buffer_put(struct ringBufQueue *_this, unsigned char towhere, unsigned int towhat) {

	group.integer = towhat;

	if (_this->count < (BUFFER_SIZE_QUEUE - 3)) {
		_this->buf[_this->head] = towhere;
		_this->head = Queue_buffer_modulo_inc(_this->head, BUFFER_SIZE_QUEUE);
		++_this->count;
		_this->buf[_this->head] = group.parts[0];
		_this->head = Queue_buffer_modulo_inc(_this->head, BUFFER_SIZE_QUEUE);
		++_this->count;
		_this->buf[_this->head] = group.parts[1];
		_this->head = Queue_buffer_modulo_inc(_this->head, BUFFER_SIZE_QUEUE);
		++_this->count;

	}

}


//pulls info out of the send buffer in a first in first out fashion

unsigned char Queue_buffer_get(struct ringBufQueue* _this) {
	unsigned char c;
	if (_this->count > 0) {
		c = _this->buf[_this->tail];
		_this->tail = Queue_buffer_modulo_inc(_this->tail, BUFFER_SIZE_QUEUE);
		--_this->count;
		} else {
		c = 0;
	}
	return (c);
}

void *memset(void *s, int c, size_t n);

//flushes the send buffer to get it ready for new data

void Queue_buffer_flush(struct ringBufQueue* _this, const int clearBuffer) {
	_this->count = 0;
	_this->head = 0;
	_this->tail = 0;
	if (clearBuffer) {
		memset(_this->buf, 0, sizeof (_this->buf));
	}
}


// increments counters for the buffer functions

unsigned int Queue_buffer_modulo_inc(const unsigned int value, const unsigned int modulus) {
	unsigned int my_value = value + 1;
	if (my_value >= modulus) {
		my_value = 0;
	}
	return (my_value);
}
