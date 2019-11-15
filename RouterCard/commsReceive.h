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
#define LEFTMOTORSPEED  1
#define RIGHTMOTORSPEED 2
#define ACTUATORSPEED 3
#define ARMSPEED    4
#define PLOWSPEED   5
#define MACROCOMMAND  8
#define MACROSUBCOMMAND 9

#include <stdbool.h>


void initCOMs(void);
bool manualMode(void);
void setupCommsTimers(void);
void updateComms(void);
void updateComs2(void);
void parseComms(void);
int getMacroCommand(void);
int getMacroSubCommand(void);

void setMacroCommand(int m);
void setMacroSubCommand(int ms);

#endif /* COMMSRECEIVE_H_ */