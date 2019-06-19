/*
 * Definitions.h
 *
 * Created: 4/7/2018 1:34:12 PM
 *  Author: sjc93
 */ 
// bucket Shaker
#ifndef DEFINITIONS_H
#define DEFINITIONS_H

//This file contains receive array defenitions for all processors
//universal
#define LastBoardReceived 0 

//1 Control Box
#define ControlBoxAddress 1

//2 Beacon
#define BeaconAddress 2
// ****** Data indices in (Transmit) fastTrans array ****** (to the beacon board)


//-------------------BLOCK 1-------------------
#define LIDAR_COMMAND_INDEX 1
/*IMPORTANT:
 --> All the lidar defenitions in block 1 if sent in the fast transfer index LIDAR_COMMAND_INDEX 
 will issue a command described by each #defenition
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
#define LIDAR_GET_HEADING 1
/*
Every transmission the return will be:
		0 -- The angle relative to the lidar of the of the object current position (LIDAR_OBJ_ANGLE)
		1 -- The Heading/angle that the object in view traveled (LIDAR_OBJ_HEADING)
			(From: the Position of object first transmission. TO: The current position of the object.)
		2 -- The distance the object traveled from points A to B (previously described) (LIDAR_OBJ_DISPLACEMENT)
		3 -- The distance of the object relative to the beacon(LIDAR_OBJ_MAG)*/		
#define LIDAR_GET_LEAPFROG_HEADING 2

//This does the exact same thing as the #define LIDAR_GET_LEAPFROG_HEADING except that it will transmit the data on a timer.
#define LIDAR_CONTINUOUS_LEAPFROG_OBJ_CAPTURE 3
//This will return the Object's angle and Magnitude
#define LIDAR_CONTINUOUS_OBJ_CAPTURE 4
#define LIDAR_GET_CONTINUOUS_HEADING 4
//----------------END OF BLOCK 1-------------------

// *******************************************
// ****** Data indices in (Recieve) fastTrans array ****** (from the beacon board)
#define LIDAR_OBJ_ANGLE 0
#define LIDAR_OBJ_HEADING 1
#define LIDAR_OBJ_AtoB_DIS 2
#define LIDAR_OBJ_MAG 3


// *******************************************

//3 Debugger
#define DebuggerAddress 3

//4 Router Card
#define RouterCardAddress 4

//5 Navigation PIC
#define NavigationAddress 5
#define maxDataAddressNavigation 50

//6 Sensor PIC	
#define SensorAddress 		6

//7 Mouse and Gyro PIC
#define MouseGyroAddress 	7

//8 Electro Statics/Bucket Shaker
#define BucketAddress 		8

//9 Bumper Switch/motor controller Left
#define BumperSwitchLeft 	10
#define LeftBumperAddress	10

//10 Bumper Switch/Motor Controller Right
#define BumperSwitchRight 	9
#define RightBumperAddress  9

//E-statics address
#define ElectrostaticsAddress 11
//-------------------[GYRO/BUMPER BOARD] CAN COMMAND ADDRESSING & INDEXs------------------

#define ControlBoxAddressFT 5

//CAN INDEXs

#define MOTOR_COMMAND_MODE_DATA_ADDRESS			1
	//****************Data In Index******************
	#define VELOCITY_MODE						1
	#define ENCODER_DISTANCE_MODE				2
	
#define MOTOR_ENCODER_DISTANCE_DATA_ADDRESS		2
#define MOTOR_VELOCITY_DATA_ADDRESS				2
#define MOTOR_PAUSE_COMMAND_ADDRESS				3
	//****************Data In Index******************
	#define REQUEST_ENCODER_VAL					3
	#define PAUSE								2
	#define RESUME								1
	#define NO_PAUSE_COMMAND					0
	
#define MOTOR_PAUSE_DATA_ADDRESS				4
	
	


//-------------------------------------------------------------------

//--------------------GYRO/BUMPER BOARD MACRO DEFINITIONS------------------------------


#define MACRO_FINALIZED_COMMAND_INDEX			7


#endif