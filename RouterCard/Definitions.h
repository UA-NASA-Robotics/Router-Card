/*
 * File:   Definitions.h
 * Author: John
 *
 * Created on May 5, 2018, 1:16 PM
 */
//**********************************Master Controller*********************8
#ifndef DEFINITIONS_H
#define DEFINITIONS_H


#define ControlBoxAddress     1

#define ROUTER_ADDRESS        4

#define MOTOR_ADDRESS       6

#define GYRO_ADDRESS        7

#define UART_COMMAND_INDEX      8
#define UART_COMMAND_DATA_INDEX   9



//#define CAN_COMMAND_INDEX       8
//#define CAN_COMMAND_DATA_INDEX  9

#define ROTATION_COMMAND        1  //rotate
#define ENCODER_COMMAND         2   //drive a distance
#define DIG_COMMAND             3
#define PAUSE_COMMAND           4     //Pause the cammand that is running

typedef struct {
	int x;
	int y;
} point_t;

typedef struct {
	int angle;
	int mag;
} polar_t;


#endif  /* DEFINITIONS_H */

