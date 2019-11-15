/*
 * MotorDefines.h
 *
 * Created: 3/6/2018 11:20:59 PM
 *  Author: Zac
 */


#ifndef MOTORDEFINES_H_
#define MOTORDEFINES_H_

#define F_CPU 16000000
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <stdbool.h>
#include "../SDO.h"
#include "../can.h"
#include "../AT90CAN_UART.h"
#include "../assert.h"

typedef enum
{
	Velocity=0,
	Position

} MotorMode;

typedef struct {
	int Pos;
	int Neg;
	int Home;
} LimitSwitch_t;

typedef struct
{
	//Can Address
	char      ID;
	char      Status;
	char      MOB;
	int       MAX_RPM;
	int       MAX_CURRENT;
	unsigned long ACCEL;
	bool      isBrushless;
	LimitSwitch_t LimitSwitch;

	//bool  CANenabled;
	circular_buffer Motor_Buffer;

} Motor_t;
#define motorCount 4


Motor_t RightMotor,LeftMotor, BucketMotor,ArmMotor,PlowMotor;

#define Motor_Buffer_Size     4
#define CAN_PACKET_SIZE 8
#define RECEIVE_MASK 0x7ff

char motorBuffer[motorCount][Motor_Buffer_Size*CAN_PACKET_SIZE];

//safety constants for motor controllers
//strictly speaking the motor controllers should
//self protect, but better safe than sorry.
#define MAXRPM        3200 //3080 is safe value printed on motors, can probably push this up to 4000 without worry if needed.
#define ACCEL_CONST     1000000 //in rev/min^2, value should be between 100k and 10k *this value will probably have to be changed under load.
#define ACCEL_CONST_BRUSH 1000000 //in rev/min^2, value should be between 100k and 10k *this value will probably have to be changed under load.

//maximum current allowed through BG65x25
#define MAXCURRENTBG65 10000  //seting current limit to 10A, max is actually 20A.
#define MANUAL_MIN_SPEED 15
//from msb to lsb, LeftMotor, RightMotor, ConveyorMotor, BucketMotor
//these are the bit positions for the respective motor's error bit in MOTORSTATUS error byte
#define  LEFTMOTOR_STATUS     7
#define  RIGHTMOTOR_STATUS      6
#define  ARMMOTOR_STATUS      5
#define  BUCKETMOTOR_STATUS     4
#define  PLOWMOTOR_STATUS     3


#define ARMMOTORID 0x7C
#define ARMMOTOR_MOB 3
#define MAXCURRENTARM 5000  //this isn't the bg65 but the 75 and the current rating should be reviewed 

#define LEFTMOTORID 0x7D
#define LEFTMOTOR_MOB 0

#define RIGHTMOTORID 0x7E
#define RIGHTMOTOR_MOB 1

#define BUCKETMOTORID 0x7F
#define BUCKETMOTOR_MOB 2
#define MAXCURRENTBUCKET MAXCURRENTBG65

#define PLOWMOTORID 0x7B
#define PLOWMOTOR_MOB 4
#define MAXCURRENTPLOW 3500





#define COMMSTIMEOUT 100 //ms, longest delay I have seen between message and reply is 68ms
#define COMMSCHECKTIME 1 //ms, how long between checking comms during timeout period

#define  CLEAR_ERRORS               0x3000, 0x00, 0x01
#define  SET_MOTOR_BRUSHED          0x3900, 0x00, 0x00
#define  SET_MOTOR_BRUSHLESS        0x3900, 0x00, 0x01
#define  SET_POLES                  0x3910, 0x00, 10
#define  SET_POLES_BG75             0x3910, 0x00, 8

#define  MOTOR_POLARITY             0x3911, 0x00, 0x02
#define  MOTOR_POLARITY_bg75        0x3911, 0x00, 0x00
#define  SET_FEEDBACK_ENCODER       0x3350, 0x00, 2410 //2410 for encoder
#define  SET_FEEDBACK_EMF           0x3350, 0x00, 274 //2410 for encoder
#define  SET_FEEDBACK_HALL          0x3350, 0x00, 2378 //2378 for hall
#define  SET_SVEL_FEEDBACK_ENCODER  0x3550, 0x00, 2410 //2410 for encoder
#define  SET_SVEL_FEEDBACK_HALL     0x3550, 0x00, 2378 //2378 for hall
#define  SET_FEEDBACK_RESOLUTION    0x3962, 0x00, 500
#define  SET_FEEDBACK_RESOLUTION_BG75 0x3962, 0x00, 4096
#define  CURRENT_LIMIT_POS          0x3221, 0//, MAXCURRENTBG65
#define  CURRENT_LIMIT_NEG          0x3223, 0//, MAXCURRENTBG65
#define  VELOCITY_LIMIT_POS         0x3321, 0x00//, MAXRPM
#define  VELOCITY_LIMIT_NEG         0x3323, 0x00//, MAXRPM
#define  POWER_ENABLE               0x3004, 0x00, 0x01
#define  POWER_DISABLE              0x3004, 0x00, 0x00
#define  MOTOR_VOLTAGE        0x3110, 0x00, 0x00
#define  MOTOR_TEMPERATURE      0x3114, 0x00, 0x00
//*******limit switch Configs***********
#define  POSITIVE_LIMIT_SWITCH    0x3055, 0x00//, 0x130 //activates digital input 0 as positive limit switch high active
#define  NEGATIVE_LIMIT_SWITCH    0x3056, 0x00//, 0x132 //activates digital input 2 as negative limit switch high active

#define  LIM_D0H          0x130   //digital input 0 as limit switch active high
#define  LIM_D1H          0x131   //digital input 1 as limit switch active high
#define  LIM_D2H          0x132   //digital input 2 as limit switch active high
#define  LIM_D0L          -0x130    //digital input 0 as limit switch active low
#define  LIM_D1L          -0x131    //digital input 1 as limit switch active low
#define  LIM_D2L          -0x132    //digital input 2 as limit switch active low
//*******End of Limit switch Configs*****
#define  MODE_VEL                   0x3003, 0x00, 0x3
#define  VEL_ACC                    0x3380, 0x00, ACCEL_CONST
#define  VEL_DEC                    0x3381, 0x00, ACCEL_CONST

//motor status tracking
int MOTORSTATUS; //assume all motors are innocent until proven guilty

#endif /* MOTORDEFINES_H_ */