#ifndef FT_CAN_H
#define FT_CAN_H

#include <stdbool.h>
#include <stdint.h>
#include "FastTransfer/ring_buffer.h"
#include "CommsIDs.h"
#include "CommsDefenition.h"
#include "mcc_generated_files/can_types.h"
#include "mcc_generated_files/can1.h"

#define GLOBAL_SYSTEM_DATA_SIZE 5

#define MY_ADDRESS 4

#define CAN_RECIEVE_SIZE   10
#define GBL_CAN_RECIEVE_SIZE   100
#define CAN_BYTE_ARRAY_LENGTH 8

#define LAST_BOARD_RECEIEVED 0
#define Instant  0
#define WhenReceiveCall  1


#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    FT_LOCAL = 0,
    FT_GLOBAL
} FT_Type_t;

void setCANFTdata(int index, int val,bool isGlobal);
int getCANFTdatas(int index,bool _isGlobal);
bool getCANFT_Flag(bool *receiveArray, int index);

int getCANFTdata(int c);     //getCANFTdatas(c,false)
int getGBL_CANFTdata(int c);   //getCANFTdatas(c,true)

bool getCANFT_RFlag(int c); //getCANFT_Flag(CAN_FT_recievedFlag,c)
bool getGBL_CANFTFlag(int c); //getCANFT_Flag(GBL_CAN_FT_recievedFlag,c)


int GlobalAddressInterpret(int index);

void beginCANFast(volatile int * ptr, volatile bool *flagPtr, unsigned int maxSize, unsigned char givenAddress, FT_Type_t _t);
void setNewDataFlag(FT_Type_t _t, int index);

//RX functions
void SetReceiveMode(int input);
//int  ReceiveDataCAN(void);


//TX functions
void ToSendCAN( unsigned int where, unsigned int what);
void ToSendCAN_Control(unsigned char where, unsigned int what);
void ToSendCAN_Beacon(unsigned char where, unsigned int what);
void sendDataCAN( unsigned int whereToSend);
void sendDataCAN_Control( unsigned int whereToSend);
void sendDataCAN_Beacon( unsigned int whereToSend);
int GetTransmitErrorCount(void);

void initCANFT(void);
int ReceiveDataCAN(FT_Type_t _t);
int * getReceiveArrayCAN(void);
void ToSendCAN(unsigned int where, unsigned int what);
void sendDataCAN(unsigned int whereToSend);
void ReceiveCANFast(uCAN_MSG *p, FT_Type_t _t); // interrupt callback
bool TransmitCANFast(uCAN_MSG *p); // interrupt callback
int getCANFastData(FT_Type_t _t, uint8_t index);
void clearCANFastDataValue(int index);
void clearCANFastDataValueRange(int startIndex,int end);
bool getNewDataFlagStatus(FT_Type_t _t, int index);

void CAN_FrameToArray(uint8_t* msg_array, uCAN_MSG* msg);
void SetCANFrameData(uCAN_MSG* msg, uint8_t* arr);
void SetCANFrameDataByte(uCAN_MSG* msg, uint8_t* arr, uint8_t index);

uCAN_MSG BufferToMSG(struct ring_buffer_t* buf, FT_Type_t addr, uint32_t where);

#ifdef __cplusplus
}
#endif
#endif