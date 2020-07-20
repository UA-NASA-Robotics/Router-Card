/*
 * commsReceive.h
 *
 * Created: 4/26/2017 2:22:33 PM
 *  Author: Seth Carpenter
 *
 *
 * System wiki : https://github.com/UA-NASA-Robotics/ROCKEE_Router-Card/wiki
 */


#ifndef COMMSRECEIVE_H_
#define COMMSRECEIVE_H_

//receive array definitions.
#define LASTBOARD   0
#define DRIVE_MOTOR_SPEED  1
#define ACTUATORSPEED 3
#define ARMSPEED    4
#define PLOWSPEED   5
#define MACROCOMMAND  8
#define MACROSUBCOMMAND 9

#include <stdbool.h>
#include "FastTransfer/ft.h"
#include "FastTransfer_CAN.h"


void CommunicationsHandle(void);

void initCOMs(void);
void setupCommsTimers(void);
void updateComms(void);
void updateComs2(void);



#endif /* COMMSRECEIVE_H_ */