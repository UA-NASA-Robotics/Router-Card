/*
 * File:   Definitions.h
 * Author: Seth Carpenter
 *
 *
 * System wiki : https://github.com/UA-NASA-Robotics/ROCKEE_Router-Card/wiki
 *
 * Created on May 5, 2018, 1:16 PM
 */
#ifndef DEFINITIONS_H
#define DEFINITIONS_H
#ifndef F_CPU
#define F_CPU 16000000
#endif
#include <util/delay.h>
/****************** SYSTEM ADDRESS ******************/
typedef enum {
	CONTROL_BOX_ADDRESS=1,
	/* -2- */
	POZYX_ADDRESS,
	/* -3- */
	JUICE_BOARD_ADDRESS,
	/* -4- */
	ROUTER_ADDRESS,
	/* -5- */
	MASTER_ADDRESS,
	/* -6- */
	MOTOR_ADDRESS,
	/* -7- */
	GYRO_ADDRESS,
	/* -8- */
	STRAINSENSOR_ADDRESS,
	/* -9- */
	OPTICAL_FLOW_ADDRESS,
	/* -10- */
	RASPBERRYPI_ADDRESS,
	/* -11- */
	LED_CARD_ADDRESS,

	GLOBAL_ADDRESS = 31
} SystemAddress_t;



#define UART_COMMAND_INDEX      8
#define UART_COMMAND_DATA_INDEX   9



//#define CAN_COMMAND_INDEX       8
//#define CAN_COMMAND_DATA_INDEX  9

#define ROTATION_COMMAND        1   //rotate
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

