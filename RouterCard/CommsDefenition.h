#ifndef COMMS_DEFINITIONS_H
#define COMMS_DEFINITIONS_H

//This file contains receive array definitions for all processors
//universal
#define LastBoardReceived 0

//1 Control Box
//#define ControlBoxAddress 1

//2 Beacon
#define BeaconAddress 2
// ****** Data indices in (Transmit) fastTrans array ****** (to the beacon board)


//-------------------BLOCK 1-------------------
#define LIDAR_COMMAND_INDEX 0
/*IMPORTANT:
 --> All the lidar defenitions in block 1 if sent in the fast transfer index LIDAR_COMMAND_INDEX
 will issue a command described by each #definition
*/
// The beacon will Transmit no data (idle state)
#define LIDAR_IDLE_STATE 0
/*
 --> The FIRST transmission of this value in the COMMAND_INDEX will instruct the beacon board to store the
     position of the Object in view.
 --> The SECOND transmission will return:
    0 -- The angle relative to the lidar of the of the object current position (LIDAR_OBJ_ANGLE)
    1 -- The Heading/angle that the object in view traveled (LIDAR_OBJ_HEADING)
      (From: the Position of object first transmission. TO: The current position of the object.)
    2 -- The distance the object traveled from points A to B (previously described) (LIDAR_OBJ_DISPLACEMENT)
    3 -- The distance of the object relative to the beacon(LIDAR_OBJ_MAG) */
#define LIDAR_GET_DATA 1
/*
Every transmission the return will be:
    0 -- The angle relative to the lidar of the of the object current position (LIDAR_OBJ_ANGLE)
    1 -- The Heading/angle that the object in view traveled (LIDAR_OBJ_HEADING)
      (From: the Position of object first transmission. TO: The current position of the object.)
    2 -- The distance the object traveled from points A to B (previously described) (LIDAR_OBJ_DISPLACEMENT)
    3 -- The distance of the object relative to the beacon(LIDAR_OBJ_MAG)*/
#define LIDAR_GET_LEAPFROG_Data 2

//This does the exact same thing as the #define LIDAR_GET_LEAPFROG_HEADING except that it will transmit the data on a timer.
#define LIDAR_CONTINUOUS_LEAPFROG_OBJ_CAPTURE 3
//This will return the Object's angle and Magnitude
#define LIDAR_CONTINUOUS_OBJ_CAPTURE 4

#define  LIDAR_GET_StartingVal    5
//----------------END OF BLOCK 1-------------------

// *******************************************
// ****** Data indices in (Recieve) fastTrans array ****** (from the beacon board)
#define LIDAR_OBJ_ANGLE     1
#define LIDAR_OBJ_MAG     2
#define LIDAR_OBJ_HEADING   3
#define LIDAR_OBJ_AtoB_DIS    4
#define LIDAR_OBJ_X       5
#define LIDAR_OBJ_Y       6
#define LIDAR_OBJ_StartInfo   7


// *******************************************
#define ControlBoxAddressFT 1
//3 Debugger
#define DebuggerAddress 3

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

//9 Bumper Switch/motor controller Left
#define BumperSwitchLeft  10
#define LeftBumperAddress 10

//10 Bumper Switch/Motor Controller Right
#define BumperSwitchRight   9
#define RightBumperAddress  9

//E-statics address
#define ElectrostaticsAddress 11
//-------------------[GYRO/BUMPER BOARD] CAN COMMAND ADDRESSING & INDEXs------------------



//CAN INDEXs

#define CAN_COMMAND_INDEX       8
#define MACRO_COMMAND_INDEX   CAN_COMMAND_INDEX
#define CAN_COMMAND_DATA_INDEX  9


#define SENDER_ADDRESS_INDEX          0

#define STOP_MACRO                0
#define MOTOR_COMMAND_MODE_DATA_ADDRESS     1
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