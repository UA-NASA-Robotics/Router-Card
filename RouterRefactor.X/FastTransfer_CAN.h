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
    
// fast transfer handle and all necessary local variables
struct FastTransferHandle_CAN
{
    // address of device
    uint8_t address;
    // can module number (1 or 2)
    int8_t can_id;
    bool(*transmit)(CAN_TX_PRIOIRTY,uCAN_MSG*);
    bool(*receive)(uCAN_MSG*);
    struct ring_buffer_t transmit_buffer_CAN, send_buffer_CAN_FT, rx_buffer_CAN;
    struct ring_buffer_t rx_buffer_CAN_Global;
    int receiveMode;
    bool dataReceived[2];
    int newDataFlag;
    volatile int * receiveArrayAddressCAN[2];
    volatile bool * receiveArrayAddressCAN_Flag[2];
    unsigned char moduleAddressCAN[2];
    unsigned int MaxIndex[2];
    int receiveArrayCAN[10];
    bool receiveArrayCAN_Flag[10];
    int newDataFlag_Global[2];
    volatile int receiveArrayCAN_Global[GLOBAL_DEVICES*GLOBAL_DATA_INDEX_PER_DEVICE + 1];
    bool GBL_CAN_FT_recievedFlag[GLOBAL_DEVICES*GLOBAL_DATA_INDEX_PER_DEVICE + 1];
    bool CAN_FT_recievedFlag[CAN_RECIEVE_SIZE];
    int ReceiveCAN[CAN_RECIEVE_SIZE];
};
typedef struct FastTransferHandle_CAN FTC_t;

/* 
    To use Fast Transfer CAN, you must pass in your hardware-level init, tx, and rx functions into here. 
    If you are using the Microchip Code Configurator for CAN, the process goes like this:
    1) declare a global FTC_t* pointer somewhere in the CAN implementation file
    2) edit the initialization function to take your FTC_t* pointer and in the function set the global FTC_t* pointer to the one you passed inside the intitialization function
    3) don't forget to remove the call to the initialization function for CAN in SYSTEM_Initialize as Fast Transfer will do this internally
    This process is to enable Fast Transfer functions to be used as interrupt callbacks for any CAN module
*/

void FTC_Init(FTC_t* handle, uint8_t address, int8_t can_module_number, void(*mcc_init)(struct FastTransferHandle_CAN*), bool(*mcc_tx)(CAN_TX_PRIOIRTY, uCAN_MSG*), bool(*mcc_rx)(uCAN_MSG*));

void FTC_ToSend(FTC_t* handle, unsigned int index, unsigned int data);
void FTC_Send(FTC_t* handle, unsigned int address);
void FTC_Receive(FTC_t* handle, uCAN_MSG* msg, FT_Type_t _t);

bool IsFIFOIE(FTC_t* handle);
void FIFOI_Enable(FTC_t* handle);
void FIFOI_Disable(FTC_t* handle);


void setCANFTdata(FTC_t* handle, int index, int val,bool isGlobal);
int getCANFTdatas(FTC_t* handle, int index, bool _isGlobal);
bool getCANFT_Flag(bool *receiveArray, int index);

int getCANFTdata(FTC_t* handle, int c);
int getGBL_CANFTdata(FTC_t* handle, int c);

bool getCANFT_RFlag(FTC_t* handle, int c);
bool getGBL_CANFTFlag(FTC_t* handle, int c);


int GlobalAddressInterpret(FTC_t* handle, int index);

void beginCANFast(FTC_t* handle, volatile int * ptr, volatile bool *flagPtr, unsigned int maxSize, unsigned char givenAddress, FT_Type_t _t);
void setNewDataFlag(FTC_t* handle, FT_Type_t _t, int index);

//RX functions
void SetReceiveMode(FTC_t* handle, int input);

//TX functions
void ToSendCAN( unsigned int where, unsigned int what);
void ToSendCAN_Control(unsigned char where, unsigned int what);
void ToSendCAN_Beacon(unsigned char where, unsigned int what);
void sendDataCAN( unsigned int whereToSend);
void sendDataCAN_Control( unsigned int whereToSend);
void sendDataCAN_Beacon( unsigned int whereToSend);
int GetTransmitErrorCount(void);

void initCANFT(FTC_t* handle);
int ReceiveDataCAN(FTC_t* handle, FT_Type_t _t);
int * getReceiveArrayCAN(FTC_t* handle);
void ToSendCAN(unsigned int where, unsigned int what);
void sendDataCAN(unsigned int whereToSend);
//void ReceiveCANFast(FTC_t* handle, uCAN_MSG *p, FT_Type_t _t); // interrupt callback
bool TransmitCANFast(FTC_t* handle, uCAN_MSG *p); // interrupt callback
int getCANFastData(FTC_t* handle, FT_Type_t _t, uint8_t index);
void clearCANFastDataValue(FTC_t* handle, int index);
void clearCANFastDataValueRange(FTC_t* handle, int startIndex,int end);
bool getNewDataFlagStatus(FTC_t* handle, FT_Type_t _t, int index);

void CAN_FrameToArray(uint8_t* msg_array, uCAN_MSG* msg);
void SetCANFrameData(uCAN_MSG* msg, uint8_t* arr);
void SetCANFrameDataByte(uCAN_MSG* msg, uint8_t* arr, uint8_t index);

uCAN_MSG BufferToMSG(struct ring_buffer_t* buf, uint32_t sender, uint32_t where);

#ifdef __cplusplus
}
#endif
#endif