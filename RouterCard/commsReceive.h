/*
 * commsReceive.h
 *
 * Created: 4/26/2017 2:22:33 PM
 *  Author: Zac
 */ 


#ifndef COMMSRECEIVE_H_
#define COMMSRECEIVE_H_

//receive array definitions.
#define LASTBOARD		0
#define LEFTMOTORSPEED	1
#define RIGHTMOTORSPEED 2
#define ACTUATORSPEED	3
#define ARMSPEED		4
#define MACROCOMMAND	5
#define MACROSUBCOMMAND 6

#include <stdbool.h>

bool manualMode(void);
void setupCommsTimers(void);
void updateComms(void);
void parseComms(void);
int getMacroCommand(void);
int getMacroSubCommand(void);
int getLeftMotorCommand(void);
int getRightMotorCommand(void);
int getArmMotorCommand(void);
int getBucketMotorCommand(void);
void updateCommsBeacon(void);
void setMacroCommand(int m);
void setMacroSubCommand(int ms);

#endif /* COMMSRECEIVE_H_ */