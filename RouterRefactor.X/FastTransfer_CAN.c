#include "FastTransfer_CAN.h"


//0b0111 1111 11xx xxxx (where the x's are your address)

//#define DEBUG


#ifdef DEBUG
#include <stdio.h>
#endif

void FTC_Init(FTC_t* handle, uint8_t address, int8_t id, void(*mcc_init)(FTC_t*), bool(*f_tx)(CAN_TX_PRIOIRTY, uCAN_MSG*), bool(*f_rx)(uCAN_MSG*)) {
    handle->address = address;
    if (id == 1 || id == 2)
    {
        handle->can_id = id;
        handle->transmit = f_tx;
        handle->receive = f_rx;
        handle->receiveMode = 0;
        handle->dataReceived[0] = false;
        handle->dataReceived[1] = false;
        handle->newDataFlag = 0;
        initCANFT(handle);
        mcc_init(handle);
    }
}

bool IsFIFOIE(FTC_t* handle) {
    if (handle->can_id == 1)
    {
        if (C1INTEbits.FIFOIE == 1)
            return true;
        else
            return false;
    }
    else if (handle->can_id == 2)
    {
        if (C2INTEbits.FIFOIE == 1)
            return true;
        else
            return false;
    }
    else return true;
}

void FIFOI_Enable(FTC_t* handle) {
    if (handle->can_id == 1)
        C1INTEbits.FIFOIE = 1;
    else if (handle->can_id == 2)
        C2INTEbits.FIFOIE = 1;
}

void FIFOI_Disable(FTC_t* handle) {
    if (handle->can_id == 1)
        C1INTEbits.FIFOIE = 0;
    else if (handle->can_id == 2)
        C2INTEbits.FIFOIE = 0;
}

void setCANFTdata(FTC_t* handle, int index, int val, bool isGlobal) {
    if (isGlobal) {
        handle->receiveArrayCAN_Global[index] = val;
        handle->GBL_CAN_FT_recievedFlag[index] = true;
        //printf("rx: %d\n",index);

    } else {

        (handle->receiveArrayAddressCAN[FT_LOCAL])[index] = val;
        handle->CAN_FT_recievedFlag[index] = true;
    }

}

int getCANFTdatas(FTC_t* handle, int index, bool _isGlobal) {
    if (_isGlobal)
        return handle->receiveArrayCAN_Global[index];
    else
        return (handle->receiveArrayAddressCAN[FT_LOCAL])[index];
    return 0;
}

bool getCANFT_Flag(volatile bool* receiveArray, int index) {
    if (receiveArray[index] == false) {
        return false;
    } else {
        receiveArray[index] = false;
        return true;
    }
}

int getCANFTdata(FTC_t* handle, int c) {
    return getCANFTdatas(handle, c, false);
}

int getGBL_CANFTdata(FTC_t* handle, int c) {
    return getCANFTdatas(handle, c, true);
}

bool getGBL_CANFTFlag(FTC_t* handle, int c) {
    return getCANFT_Flag(handle->receiveArrayAddressCAN_Flag[FT_GLOBAL], c);
}

bool getCANFT_RFlag(FTC_t* handle, int c) {
    return getCANFT_Flag(handle->receiveArrayAddressCAN_Flag[FT_LOCAL], c);
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
    switch (index) {
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

int * getReceiveArrayCAN(FTC_t* handle) {
    return handle->receiveArrayCAN;
}

void initCANFT(FTC_t* handle) {
    beginCANFast(handle, handle->receiveArrayCAN, handle->CAN_FT_recievedFlag, sizeof (handle->receiveArrayCAN), handle->address, FT_LOCAL);
    beginCANFast(handle, handle->receiveArrayCAN_Global, handle->GBL_CAN_FT_recievedFlag, sizeof (handle->receiveArrayCAN_Global), GLOBAL_ADDRESS, FT_GLOBAL);
}
void beginCANFast(FTC_t* handle, volatile int * ptr, volatile bool *flagPtr, uint16_t maxSize, uint8_t givenAddress, FT_Type_t _t) {

    handle->receiveArrayAddressCAN[_t] = ptr;
    handle->receiveArrayAddressCAN_Flag[_t] = flagPtr;
    handle->moduleAddressCAN[_t] = givenAddress;
    handle->MaxIndex[_t] = maxSize;

    if (_t == FT_GLOBAL) {
        rbuffer_reset(&handle->rx_buffer_CAN_Global);
    } else {
        rbuffer_reset(&handle->rx_buffer_CAN);
        rbuffer_reset(&handle->send_buffer_CAN_FT);
        rbuffer_reset(&handle->transmit_buffer_CAN);
    }
}

void setNewDataFlag(FTC_t* handle, FT_Type_t _t, int index) {
    (handle->receiveArrayAddressCAN_Flag[_t])[index] = 1;
}

void clearNewDataFlag(FTC_t* handle, FT_Type_t _t, int index) {
    (handle->receiveArrayAddressCAN_Flag[_t])[index] = 0;
}

bool getNewDataFlagStatus(FTC_t* handle, FT_Type_t _t, int index) {
    if ((handle->receiveArrayAddressCAN_Flag[_t])[index]) {
        clearNewDataFlag(handle, _t, index);
        return true;
    } else {
        return false;
    }
}

int getCANFastData(FTC_t* handle, FT_Type_t _t, uint8_t index) {
    ReceiveDataCAN(handle, _t);
    if (index < handle->MaxIndex[_t]) {

        return (handle->receiveArrayAddressCAN[_t])[index];
    }
    return 0xFFFF;
}

void clearCANFastDataValue(FTC_t* handle, int index) {

    handle->receiveArrayCAN[index] = 0;
}

void clearCANFastDataValueRange(FTC_t* handle, int startIndex, int end) {
    if (startIndex > end) {
        int index = startIndex;
        startIndex = end;
        end = index;
    }
    int i;
    for (i = startIndex; i <= end; i++) {

        handle->receiveArrayCAN[i] = 0;
        clearNewDataFlag(handle, FT_LOCAL, i);
    }
}

//void ReceiveCANFast(FTC_t* handle, uCAN_MSG* p, FT_Type_t _t) // interrupt callback
void FTC_Receive(FTC_t* handle, uCAN_MSG* p, FT_Type_t _t)
{
    struct ring_buffer_t* rx_Buff;
    if (_t == FT_LOCAL)
        rx_Buff = &handle->rx_buffer_CAN;
    else
        rx_Buff = &handle->rx_buffer_CAN_Global;

    if ((p->frame.id >> 6) == handle->moduleAddressCAN[_t]) {
        if (p->frame.dlc == 8 || p->frame.dlc == 9) //Check number of bytes, if 8 read in two ints
        {
            if ((p->frame.data0 << 8) + (p->frame.data1) < handle->MaxIndex[_t]) {
                rbuffer_push2(rx_Buff, (p->frame.data0 << 8) + (p->frame.data1), (p->frame.data2 << 8) + (p->frame.data3));
                handle->dataReceived[_t] = 1;
            }
            if ((p->frame.data4 << 8) + (p->frame.data5) < handle->MaxIndex[_t]) {
                rbuffer_push2(rx_Buff, (p->frame.data4 << 8) + (p->frame.data5), (p->frame.data6 << 8) + (p->frame.data7));
                handle->dataReceived[_t] = 1;
            }
            rbuffer_push2(rx_Buff, LAST_BOARD_RECEIEVED, (p->frame.id & 0b11111));
        } else //else read in one int
        {
            if ((p->frame.data0 << 8) + (p->frame.data1) < handle->MaxIndex[_t]) {

                rbuffer_push2(rx_Buff, (p->frame.data0 << 8) + (p->frame.data1), (p->frame.data2 << 8) + (p->frame.data3));
                handle->dataReceived[_t] = 1;
            }
            rbuffer_push2(rx_Buff, LAST_BOARD_RECEIEVED, (p->frame.id & 0b11111));
        }
    }
}

int ReceiveDataCAN(FTC_t* handle, FT_Type_t _t) {
//int FTC_Read(FTC_t* handle, FT_Type_t _t) {
    struct ring_buffer_t *rx_Buff;
    if (_t == FT_LOCAL) rx_Buff = &handle->rx_buffer_CAN;
    else rx_Buff = &handle->rx_buffer_CAN_Global;

    if (handle->dataReceived[_t]) {
        handle->dataReceived[_t] = 0;
        int i = rbuffer_size(rx_Buff);
        if (i) //this better be true ... if not in instant receive 
        {
            for (; i > 0; i = i - 2) {
                int address = rbuffer_pop(rx_Buff);
                (handle->receiveArrayAddressCAN[_t])[address] = rbuffer_pop(rx_Buff);
                setNewDataFlag(handle, _t, address);
            }
            return 1;
        } else {
            //error (how was ReceiveData true if no data available)
            return 0;
        }

    } else
        return 0;
}

bool TransmitCANFast(FTC_t* handle, uCAN_MSG *p) // interrupt callback
{
    if (rbuffer_size(&handle->transmit_buffer_CAN) > 0) {
        // get array-equivalent of data byte portion of uCAN_MSG frame structure
        uint8_t msg_arr[CAN_BYTE_ARRAY_LENGTH];
        CAN_FrameToArray(msg_arr, p);
        //if more than 2 data/index pairs left might be able to send large packet. 
        if (rbuffer_size(&handle->transmit_buffer_CAN) > 6) {
            uint16_t address = rbuffer_pop(&handle->transmit_buffer_CAN);
            p->frame.id = (address << 6) + handle->address; //MY_ADDRESS; //not passed through messages will have wrong sender address
            //we are good to send the first index/value pair for sure. 
            int i = 0;
            for (i = 0; i < 2; i++) {
                uint16_t temp = rbuffer_pop(&handle->transmit_buffer_CAN);
                msg_arr[2 * i] = (temp >> 8);
                msg_arr[2 * i + 1] = temp;
            }
            //now we need to check that the next address matches the current address
            if (rbuffer_peek(&handle->transmit_buffer_CAN) == address) {
                //address match and we are clear to send them in a single packet
                rbuffer_pop(&handle->transmit_buffer_CAN); //call this to clear out the address
                int i = 0;
                for (i = 2; i < 4; i++) {
                    uint16_t temp = rbuffer_pop(&handle->transmit_buffer_CAN);
                    msg_arr[2 * i] = (temp >> 8);
                    msg_arr[2 * i + 1] = temp;
                }
                //need to check if this was the last packet 
                if (rbuffer_peek(&handle->transmit_buffer_CAN) == address) {
                    //next address is same, so this isn't the last packet, send a length 8.
                    p->frame.dlc = 8;
                } else {
                    //next address is different, and this is the last packet, so send a 9.
                    p->frame.dlc = 9;
                }
            } else {
                //addresses don't match and we should just send what is currently in the packet. 
                p->frame.dlc = 4;
            }
            SetCANFrameData(p, msg_arr);
        }            //if exactly 2 data/index pairs left send with length 9. Receiver
            //will read the "wrong" length correctly, but realize this is the last packet.
            //note: still need to check incase two different destinations. 
        else if (rbuffer_size(&handle->transmit_buffer_CAN) == 6) {
            uint16_t address = rbuffer_pop(&handle->transmit_buffer_CAN);
            p->frame.id = (address << 6) + handle->address;//MY_ADDRESS; //not passed through messages will have wrong sender address
            //we are good to send the first index/value pair for sure.
            int i = 0;
            for (i = 0; i < 2; i++) {
                uint16_t temp = rbuffer_pop(&handle->transmit_buffer_CAN);
                msg_arr[2 * i] = (temp >> 8); //this is so dumb
                msg_arr[2 * i + 1] = temp;
            }
            //now we need to check that the next address matches the current address
            if (rbuffer_peek(&handle->transmit_buffer_CAN) == address) {
                //address match and we are clear to send them in a single packet
                rbuffer_pop(&handle->transmit_buffer_CAN); //call this to clear out the address
                int i = 0;
                for (i = 2; i < 4; i++) {
                    uint16_t temp = rbuffer_pop(&handle->transmit_buffer_CAN);
                    msg_arr[2 * i] = (temp >> 8);
                    msg_arr[2 * i + 1] = temp;
                }
                p->frame.dlc = 9;
            } else {
                //addresses don't match and we should just send what is currently in the packet.
                p->frame.dlc = 4;
            }
            SetCANFrameData(p, msg_arr);
        }            //if only 1 data/index pair receiver will know it is the last packet.
        else if (rbuffer_size(&handle->transmit_buffer_CAN) == 3) {
            uint16_t address = rbuffer_pop(&handle->transmit_buffer_CAN);
            p->frame.id = (address << 6) + handle->address;//MY_ADDRESS; //not passed through messages will have wrong sender address
            p->frame.dlc = 4;
            int i = 0;
            for (i = 0; i < 2; i++) {
                uint16_t temp = rbuffer_pop(&handle->transmit_buffer_CAN);
                msg_arr[2 * i] = (temp >> 8);
                msg_arr[2 * i + 1] = temp;
            }
            SetCANFrameData(p, msg_arr);
        } else {
            //error, missing dest/index/value set, previous packets could be very corrupt
            //TransmitSetMissMatch++;
            rbuffer_clear(&handle->transmit_buffer_CAN);
            return false;
        }
        return true;
    } else {
        //C1FIFOINT2bits.TXEMPTYIE = 0;
        //C1INTEbits.FIFOIE = 0;
        //handle->CAN_STRUCT_PTR->disable_fifoi();
        FIFOI_Disable(handle);
        return false;
    }
}

void SetReceiveMode(FTC_t* handle, int input) {
    if (input == Instant || input == WhenReceiveCall) {
        handle->receiveMode = input;
    }
}

void clearCANFTdataIndex(FTC_t* handle, int index) {
    handle->receiveArrayAddressCAN[index] = 0;
}

int GlobalAddressInterpret(FTC_t* handle, int index) {
    return handle->address * GLOBAL_SYSTEM_DATA_SIZE + index;
}

void FTC_ToSend(FTC_t* handle, uint16_t where, uint16_t what) {
    rbuffer_push2(&handle->send_buffer_CAN_FT, where >> 8, where);
    rbuffer_push2(&handle->send_buffer_CAN_FT, what >>8, what);
}

void FTC_Send(FTC_t* handle, uint16_t whereToSend) {
    //NEW STATIC METHOD
    //int temp = rbuffer_size(&handle->send_buffer_CAN_FT); //get size of things to send

//    int i = 0;
//    for (i = 0; i < (temp >> 1); i++) { //need to divid by two since reading index/value pairs, hence >>1
//        int index = rbuffer_pop(&send_buffer_CAN_FT);
//        int value = rbuffer_pop(&send_buffer_CAN_FT);
//        rbuffer_push3(&transmit_buffer_CAN, whereToSend, index, value);
//    }
    uCAN_MSG msg = BufferToMSG(&handle->send_buffer_CAN_FT, (uint32_t)handle->moduleAddressCAN[FT_LOCAL], (uint32_t) whereToSend);
    //CAN1_transmit(CAN_PRIORITY_HIGH, &msg);
    handle->transmit(CAN_PRIORITY_HIGH, &msg);
    /*
    if (C1INTEbits.FIFOIE == 0) {
        C1INTEbits.FIFOIE = 1;
        //C1FIFOINT2bits.TXEMPTYIE = 1;
    }
    */
   /*
   if (!handle->CAN_STRUCT_PTR->isfifoie())
   {
       handle->CAN_STRUCT_PTR->enable_fifoi();
   }
   */
    if (!IsFIFOIE(handle))
   {
       FIFOI_Enable(handle);
   }
}

uCAN_MSG BufferToMSG(struct ring_buffer_t* buf, uint32_t sender, uint32_t whereToSend) {
    uint8_t size = (uint8_t) rbuffer_size(buf);
    uCAN_MSG t;

    t.frame.id = (whereToSend << 6) | sender; //MOUSE_GYRO_ADDRESS; //address is the same for all following packets
    t.frame.idType = CAN_FRAME_STD;
    //Use CAN open
    //t.frame.id |= (0<<5);

    if (size >= 8) {
        t.frame.data0 = rbuffer_pop(buf);
        t.frame.data1 = rbuffer_pop(buf);
        t.frame.data2 = rbuffer_pop(buf);
        t.frame.data3 = rbuffer_pop(buf);
        t.frame.data4 = rbuffer_pop(buf);
        t.frame.data5 = rbuffer_pop(buf);
        t.frame.data6 = rbuffer_pop(buf);
        t.frame.data7 = rbuffer_pop(buf);
        t.frame.dlc = 8;
    } else if (size >= 4) {
        t.frame.data0 = rbuffer_pop(buf);
        t.frame.data1 = rbuffer_pop(buf);
        t.frame.data2 = rbuffer_pop(buf);
        t.frame.data3 = rbuffer_pop(buf);
        t.frame.dlc = 4;
    }
    return t;
}



/*
void ToSendCAN(uint16_t where, uint16_t what) {
    rbuffer_push2(&send_buffer_CAN_FT, where >> 8, where);
    rbuffer_push2(&send_buffer_CAN_FT, what >>8, what);
}

void sendDataCAN(uint16_t whereToSend) {
    //NEW STATIC METHOD
    int temp = rbuffer_size(&send_buffer_CAN_FT); //get size of things to send

//    int i = 0;
//    for (i = 0; i < (temp >> 1); i++) { //need to divid by two since reading index/value pairs, hence >>1
//        int index = rbuffer_pop(&send_buffer_CAN_FT);
//        int value = rbuffer_pop(&send_buffer_CAN_FT);
//        rbuffer_push3(&transmit_buffer_CAN, whereToSend, index, value);
//    }
    uCAN_MSG msg = BufferToMSG(&send_buffer_CAN_FT, FT_LOCAL, (uint32_t) whereToSend);
    CAN1_transmit(CAN_PRIORITY_HIGH, &msg);
    if (C1INTEbits.FIFOIE == 0) {
        C1INTEbits.FIFOIE = 1;
        //C1FIFOINT2bits.TXEMPTYIE = 1;
    }
}

uCAN_MSG BufferToMSG(struct ring_buffer_t* buf, FT_Type_t _t, uint32_t whereToSend) {
    uint8_t size = (uint8_t) rbuffer_size(buf);
    uCAN_MSG t;

    uint32_t sender;
    sender = (uint32_t)moduleAddressCAN[_t];


    t.frame.id = (whereToSend << 6) | sender; //MOUSE_GYRO_ADDRESS; //address is the same for all following packets
    t.frame.idType = CAN_FRAME_STD;
    //Use CAN open
    //t.frame.id |= (0<<5);

    if (size >= 8) {
        t.frame.data0 = rbuffer_pop(buf);
        t.frame.data1 = rbuffer_pop(buf);
        t.frame.data2 = rbuffer_pop(buf);
        t.frame.data3 = rbuffer_pop(buf);
        t.frame.data4 = rbuffer_pop(buf);
        t.frame.data5 = rbuffer_pop(buf);
        t.frame.data6 = rbuffer_pop(buf);
        t.frame.data7 = rbuffer_pop(buf);
        t.frame.dlc = 8;
    } else if (size >= 4) {
        t.frame.data0 = rbuffer_pop(buf);
        t.frame.data1 = rbuffer_pop(buf);
        t.frame.data2 = rbuffer_pop(buf);
        t.frame.data3 = rbuffer_pop(buf);
        t.frame.dlc = 4;
    }
    return t;
}
*/