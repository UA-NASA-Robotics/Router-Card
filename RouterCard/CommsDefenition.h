#ifndef COMMS_DEFINITIONS_H
#define COMMS_DEFINITIONS_H

//This file contains receive array definitions for all processors
//universal
#define LastBoardReceived 0

//1 Control Box
//#define ControlBoxAddress 1

// *******************************************
// *******************************************
// *******************************************

#define ControlBoxAddressFT 1

//4 Router Card
#define RouterCardAddress 4

//5 Master Navi PIC
#define MasterAddress 5

//#define maxDataAddressNavigation 50
#define MotorAddress  6

//7 Gyro PIC
#define GyroAddress   7

//8 Electro Statics/Bucket Shaker
#define BucketAddress     8





//CAN INDEXs

#define CAN_COMMAND_INDEX       8
#define MACRO_COMMAND_INDEX   CAN_COMMAND_INDEX
#define CAN_COMMAND_DATA_INDEX  9


#define SENDER_ADDRESS_INDEX          0

#define STOP_MACRO                0
#define UPTIME_COUNTER_INDEX      1
//****************Data In Index******************
#define VELOCITY_MODE           1
#define ENCODER_DISTANCE_MODE       2

#define MOTOR_ENCODER_DISTANCE_DATA_ADDRESS   2
#define MOTOR_VELOCITY_DATA_ADDRESS       2

#define MOTOR_PAUSE_COMMAND_ADDRESS       3
//****************Data In Index******************
#define REQUEST_ENCODER_VAL         3
#define PAUSE               2
#define RESUME                1
#define NO_PAUSE_COMMAND          0

#define MOTOR_PAUSE_DATA_ADDRESS        4


#define REQUEST_BEACON_DATA           5






//-------------------------------------------------------------------

//--------------------GYRO/BUMPER BOARD MACRO DEFINITIONS------------------------------


#define MACRO_FINALIZED_COMMAND_INDEX     7




#endif