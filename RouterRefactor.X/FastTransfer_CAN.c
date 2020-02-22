#include "FastTransfer_CAN.h"


//0b0111 1111 11xx xxxx (where the x's are your address)

//#define DEBUG


#ifdef DEBUG
#include <stdio.h>
#endif

int receiveMode = 0;

struct ring_buffer_t transmit_buffer_CAN, send_buffer_CAN_FT, rx_buffer_CAN;
struct ring_buffer_t rx_buffer_CAN_Global;


bool dataReceived[2] = {false,false};
volatile int * receiveArrayAddressCAN[2];
volatile bool * receiveArrayAddressCAN_Flag[2];
unsigned char moduleAddressCAN[2];
unsigned int MaxIndex[2];
int receiveArrayCAN[10];
bool receiveArrayCAN_Flag[10];

int newDataFlag = 0;
int newDataFlag_Global[2];


volatile int receiveArrayCAN_Global[GLOBAL_DEVICES*GLOBAL_DATA_INDEX_PER_DEVICE +1];
bool GBL_CAN_FT_recievedFlag[GLOBAL_DEVICES*GLOBAL_DATA_INDEX_PER_DEVICE +1];
bool CAN_FT_recievedFlag[CAN_RECIEVE_SIZE];
int ReceiveCAN[CAN_RECIEVE_SIZE];


void setCANFTdata(int index, int val,bool isGlobal)
{
	if(isGlobal)
	{
		receiveArrayCAN_Global[index] = val;
		GBL_CAN_FT_recievedFlag[index]=true;
		//printf("rx: %d\n",index);

	} else {

		(receiveArrayAddressCAN[FT_LOCAL])[index]=val;
		CAN_FT_recievedFlag[index] = true;
	}

}


int getCANFTdatas(int index,bool _isGlobal)
{
	if(_isGlobal)
		return receiveArrayCAN_Global[index];
	else
		return (receiveArrayAddressCAN[FT_LOCAL])[index];
    return 0;
}
bool getCANFT_Flag(bool* receiveArray, int index)
{
	if(receiveArray[index] == false) {
		return false;
	} else {
		receiveArray[index] = false;
		return true;
	}
}


int getCANFTdata(int c) {
    return getCANFTdatas(c,false);
}
int getGBL_CANFTdata(int c) {
    return getCANFTdatas(c,true);
}

bool getCANFT_RFlag(int c) {
    return getCANFT_Flag(CAN_FT_recievedFlag,c);
}
bool getGBL_CANFTFlag(int c) {
    return getCANFT_Flag(GBL_CAN_FT_recievedFlag,c);
}



void CAN_FrameToArray(uint8_t* msg_array, uCAN_MSG* msg) {
    msg_array[0] = msg->frame.data0;
    msg_array[1] = msg->frame.data1;
    msg_array[2] = msg->frame.data2;
    msg_array[3] = msg->frame.data3;
    msg_array[4] = msg->frame.data4;
    msg_array[5] = msg->frame.data5;
    msg_array[6] = msg->frame.data6;
    msg_array[7] = msg->frame.data7;
}

void SetCANFrameData(uCAN_MSG* msg, uint8_t* arr) {
    msg->frame.data0 = arr[0];
    msg->frame.data1 = arr[1];
    msg->frame.data2 = arr[2];
    msg->frame.data3 = arr[3];
    msg->frame.data4 = arr[4];
    msg->frame.data5 = arr[5];
    msg->frame.data6 = arr[6];
    msg->frame.data7 = arr[7];
}

void SetCANFrameDataByte(uCAN_MSG* msg, uint8_t* arr, uint8_t index) {
    switch(index)
    {
        case 0:
        msg->frame.data0 = arr[0];
        break;
        case 1:
        msg->frame.data1 = arr[1];
        break;
        case 2:
        msg->frame.data2 = arr[2];
        break;
        case 3:
        msg->frame.data3 = arr[3];
        break;
        case 4:
        msg->frame.data4 = arr[4];
        break;
        case 5:
        msg->frame.data5 = arr[5];
        break;
        case 6:
        msg->frame.data6 = arr[6];
        break;
        case 7:
        msg->frame.data7 = arr[7];
        break;
    }
}

int * getReceiveArrayCAN(void) {
    return receiveArrayCAN;
}

void initCANFT(void) {
    beginCANFast(receiveArrayCAN, receiveArrayCAN_Flag, sizeof (receiveArrayCAN), MY_ADDRESS,FT_LOCAL);
    beginCANFast(receiveArrayCAN_Global, GBL_CAN_FT_recievedFlag, sizeof (receiveArrayCAN_Global), GLOBAL_ADDRESS, FT_GLOBAL);
}

void beginCANFast(volatile int * ptr, volatile bool *flagPtr, unsigned int maxSize, unsigned char givenAddress, FT_Type_t _t) {

    receiveArrayAddressCAN[_t] = ptr;
    receiveArrayAddressCAN_Flag[_t] = flagPtr;
    moduleAddressCAN[_t] = givenAddress;
    MaxIndex[_t] = maxSize;

    if (_t == FT_GLOBAL) {
        rbuffer_reset(&rx_buffer_CAN_Global);
    } else {
        rbuffer_reset(&rx_buffer_CAN);
        rbuffer_reset(&send_buffer_CAN_FT);
        rbuffer_reset(&transmit_buffer_CAN);
    }
}

void setNewDataFlag(FT_Type_t _t, int index) {
    (receiveArrayAddressCAN_Flag[_t])[index] = 1;
}

void clearNewDataFlag(FT_Type_t _t, int index) {
    (receiveArrayAddressCAN_Flag[_t])[index] = 0;
}

bool getNewDataFlagStatus(FT_Type_t _t, int index) {
    if ((receiveArrayAddressCAN_Flag[_t])[index]) {
        clearNewDataFlag(_t, index);
        return true;
    } else {
        return false;
    }
}

int getCANFastData(FT_Type_t _t, uint8_t index) {
    ReceiveDataCAN(_t);
    if (index < MaxIndex[_t]) {

        return (receiveArrayAddressCAN[_t])[index];
    }
    return 0xFFFF;
}

void clearCANFastDataValue(int index) {

    receiveArrayCAN[index] = 0;
}

void clearCANFastDataValueRange(int startIndex, int end) {
    if (startIndex > end) {
        int index = startIndex;
        startIndex = end;
        end = index;
    }
    int i;
    for (i = startIndex; i <= end; i++) {

        receiveArrayCAN[i] = 0;
        clearNewDataFlag(FT_LOCAL, i);
    }
}

void ReceiveCANFast(uCAN_MSG* p, FT_Type_t _t) // interrupt callback
{
    struct ring_buffer_t* rx_Buff;
    if (_t == FT_LOCAL) 
        rx_Buff = &rx_buffer_CAN;
    else 
        rx_Buff = &rx_buffer_CAN_Global;

    if ((p->frame.id >> 6) == moduleAddressCAN[_t]) {
        if (p->frame.dlc == 8 || p->frame.dlc == 9) //Check number of bytes, if 8 read in two ints
        {
            if ((p->frame.data0 << 8) + (p->frame.data1) < MaxIndex[_t]) {
                rbuffer_push2(rx_Buff, (p->frame.data0 << 8) + (p->frame.data1), (p->frame.data2 << 8) + (p->frame.data3));
                dataReceived[_t] = 1;
            }
            if ((p->frame.data4 << 8) + (p->frame.data5) < MaxIndex[_t]) {
                rbuffer_push2(rx_Buff, (p->frame.data4 << 8) + (p->frame.data5), (p->frame.data6 << 8) + (p->frame.data7));
                dataReceived[_t] = 1;
            }
            rbuffer_push2(rx_Buff, LAST_BOARD_RECEIEVED, (p->frame.id & 0b11111));
        } else //else read in one int
        {
            if ((p->frame.data0 << 8) + (p->frame.data1) < MaxIndex[_t]) {

                rbuffer_push2(rx_Buff, (p->frame.data0 << 8) + (p->frame.data1), (p->frame.data2 << 8) + (p->frame.data3));
                dataReceived[_t] = 1;
            }
            rbuffer_push2(rx_Buff, LAST_BOARD_RECEIEVED, (p->frame.id & 0b11111));
        }
    }
}

int ReceiveDataCAN(FT_Type_t _t) {
    struct ring_buffer_t *rx_Buff;
    if (_t == FT_LOCAL) rx_Buff = &rx_buffer_CAN;
    else rx_Buff = &rx_buffer_CAN_Global;

    if (dataReceived[_t]) {
        dataReceived[_t] = 0;
        int i = rbuffer_size(rx_Buff);
        if (i) //this better be true ... if not in instant receive 
        {
            for (; i > 0; i = i - 2) {
                int address = rbuffer_pop(rx_Buff);
                (receiveArrayAddressCAN[_t])[address] = rbuffer_pop(rx_Buff);
                setNewDataFlag(_t, address);
            }
            return 1;
        } else {
            //error (how was ReceiveData true if no data available)
            return 0;
        }

    } else
        return 0;
}

bool TransmitCANFast(uCAN_MSG *p) // interrupt callback
{
    if (rbuffer_size(&transmit_buffer_CAN) > 0) {
        // get array-equivalent of data byte portion of uCAN_MSG frame structure
        uint8_t msg_arr[CAN_BYTE_ARRAY_LENGTH];
        CAN_FrameToArray(msg_arr, p);
        //if more than 2 data/index pairs left might be able to send large packet. 
        if (rbuffer_size(&transmit_buffer_CAN) > 6)
        {
            unsigned int address = rbuffer_pop(&transmit_buffer_CAN);
            p->frame.id = (address << 6) + MY_ADDRESS; //not passed through messages will have wrong sender address
            //we are good to send the first index/value pair for sure. 
            int i = 0;
            for (i = 0; i < 2; i++) {
                unsigned int temp = rbuffer_pop(&transmit_buffer_CAN);
                msg_arr[2 * i] = (temp >> 8);
                msg_arr[2 * i + 1] = temp;
            }
            //now we need to check that the next address matches the current address
            if (rbuffer_peek(&transmit_buffer_CAN) == address)
            {
                //address match and we are clear to send them in a single packet
                rbuffer_pop(&transmit_buffer_CAN); //call this to clear out the address
                int i = 0;
                for (i = 2; i < 4; i++) {
                    unsigned int temp = rbuffer_pop(&transmit_buffer_CAN);
                    msg_arr[2 * i] = (temp >> 8);
                    msg_arr[2 * i + 1] = temp;
                }
                //need to check if this was the last packet 
                if (rbuffer_peek(&transmit_buffer_CAN) == address)
                {
                    //next address is same, so this isn't the last packet, send a length 8.
                    p->frame.dlc = 8;
                }
                else
                {
                    //next address is different, and this is the last packet, so send a 9.
                    p->frame.dlc = 9;
                }
            }
            else
            {
                //addresses don't match and we should just send what is currently in the packet. 
                p->frame.dlc = 4;
            }
            SetCANFrameData(p, msg_arr);
        }
            //if exactly 2 data/index pairs left send with length 9. Receiver
            //will read the "wrong" length correctly, but realize this is the last packet.
            //note: still need to check incase two different destinations. 
        else if (rbuffer_size(&transmit_buffer_CAN) == 6)
        {
            unsigned int address = rbuffer_pop(&transmit_buffer_CAN);
            p->frame.id = (address << 6) + MY_ADDRESS; //not passed through messages will have wrong sender address
            //we are good to send the first index/value pair for sure.
            int i = 0;
            for (i = 0; i < 2; i++) {
                unsigned int temp = rbuffer_pop(&transmit_buffer_CAN);
                msg_arr[2 * i] = (temp >> 8); //this is so dumb
                msg_arr[2 * i + 1] = temp;
            }
            //now we need to check that the next address matches the current address
            if (rbuffer_peek(&transmit_buffer_CAN) == address)
            {
                //address match and we are clear to send them in a single packet
                rbuffer_pop(&transmit_buffer_CAN); //call this to clear out the address
                int i = 0;
                for (i = 2; i < 4; i++) {
                    unsigned int temp = rbuffer_pop(&transmit_buffer_CAN);
                    msg_arr[2 * i] = (temp >> 8);
                    msg_arr[2 * i + 1] = temp;
                }
                p->frame.dlc = 9;
            }
            else
            {
                //addresses don't match and we should just send what is currently in the packet.
                p->frame.dlc = 4;
            }
            SetCANFrameData(p, msg_arr);
        }
        //if only 1 data/index pair receiver will know it is the last packet.
        else if (rbuffer_size(&transmit_buffer_CAN) == 3)
        {
            unsigned int address = rbuffer_pop(&transmit_buffer_CAN);
            p->frame.id = (address << 6) + MY_ADDRESS; //not passed through messages will have wrong sender address
            p->frame.dlc = 4;
            int i = 0;
            for (i = 0; i < 2; i++) {
                unsigned int temp = rbuffer_pop(&transmit_buffer_CAN);
                msg_arr[2 * i] = (temp >> 8);
                msg_arr[2 * i + 1] = temp;
            }
            SetCANFrameData(p, msg_arr);
        }
        else
        {
            //error, missing dest/index/value set, previous packets could be very corrupt
            //TransmitSetMissMatch++;
            rbuffer_clear(&transmit_buffer_CAN);
            return false;
        }
        return true;
    }
    else
    {
        //C1FIFOINT2bits.TXEMPTYIE = 0;
        C1INTEbits.FIFOIE = 0;
        return false;
    }
}


void SetReceiveMode(int input) {
	if(input == Instant || input == WhenReceiveCall )
	{
		receiveMode = input;
	}
}

void clearCANFTdataIndex(int index)
{
	receiveArrayAddressCAN[index] = 0;
}



void ToSendCAN(unsigned int where, unsigned int what) {
    rbuffer_push2(&send_buffer_CAN_FT, where, what);
}


void sendDataCAN(unsigned int whereToSend) {
    //NEW STATIC METHOD
    int temp = rbuffer_size(&send_buffer_CAN_FT); //get size of things to send

    int i = 0;
    for (i = 0; i < (temp >> 1); i++) { //need to divid by two since reading index/value pairs, hence >>1
        int index = rbuffer_pop(&send_buffer_CAN_FT);
        int value = rbuffer_pop(&send_buffer_CAN_FT);
        rbuffer_push3(&transmit_buffer_CAN, whereToSend, index, value);
    }
    uCAN_MSG msg = BufferToMSG(&transmit_buffer_CAN, FT_LOCAL, (uint32_t)whereToSend);
    CAN1_transmit(CAN_PRIORITY_HIGH, &msg);
    if (C1INTEbits.FIFOIE == 0)
    {
        C1INTEbits.FIFOIE = 1;
        //C1FIFOINT2bits.TXEMPTYIE = 1;
    }
}



int GlobalAddressInterpret(int index) {
    return MY_ADDRESS * GLOBAL_SYSTEM_DATA_SIZE + index;
}

uCAN_MSG BufferToMSG(struct ring_buffer_t* buf, FT_Type_t _t, uint32_t whereToSend)
{
	uint8_t size = (uint8_t)rbuffer_size(buf);
    uCAN_MSG t; 

    uint32_t sender;
    if (_t == FT_LOCAL)
        sender = 4;
    else
        sender = 31;

    
    t.frame.id = (whereToSend<<6) + sender; //MOUSE_GYRO_ADDRESS; //address is the same for all following packets
    
    //Use CAN open
    //t.frame.id |= (0<<5);

    t.frame.dlc = size;

    if (size == 8)
    {
        t.frame.data0 = rbuffer_pop(buf);
        t.frame.data1 = rbuffer_pop(buf);
        t.frame.data2 = rbuffer_pop(buf);
        t.frame.data3 = rbuffer_pop(buf);
        t.frame.data4 = rbuffer_pop(buf);
        t.frame.data5 = rbuffer_pop(buf);
        t.frame.data6 = rbuffer_pop(buf);
        t.frame.data7 = rbuffer_pop(buf);
    }
    else if (size == 4)
    {
        t.frame.data0 = rbuffer_pop(buf);
        t.frame.data1 = rbuffer_pop(buf);
        t.frame.data2 = rbuffer_pop(buf);
        t.frame.data3 = rbuffer_pop(buf);
    }
    
    /*
	for(;size>0;) {
        if(size>=4) {
            //send 2 data and 2 indexes
            t.frame.dlc = 8; //length = 8;
            temp = rbuffer_pop(buf); 
            t.frame.data0 = temp>>8;
            t.frame.data1 = temp;
            temp = rbuffer_pop(buf);
            t.frame.data2 = temp>>8; 
            t.frame.data3 = temp;
            temp = rbuffer_pop(buf);
            t.frame.data4 = temp>>8; 
            t.frame.data5 = temp;
            temp = rbuffer_pop(buf);
            t.frame.data6 = temp>>8;
            t.frame.data7 = temp; 
            //can_tx(&t); 
            size -=4; 
        }
        else if(size >=2 ) {
            //send 1 data and 1 index
            t.frame.dlc = 4; 
            temp = rbuffer_pop(buf); 
            t.frame.data0 = temp>>8;
            t.frame.data1 = temp;
            temp = rbuffer_pop(buf);
            t.frame.data2 = temp>>8; 
            t.frame.data3 = temp;
            //can_tx(&t); 
            size -= 2; 
        }
        else if(size == 1) {
            //error
            //TransmitPairMissMatch++; 
        }
    }
    */
	return t;
}