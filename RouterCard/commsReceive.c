/*
 * commsReceive.c
 *
 * Created: 4/26/2017 2:22:24 PM
 *  Author: Seth Carpenter
 *
 *
 * System wiki : https://github.com/UA-NASA-Robotics/ROCKEE_Router-Card/wiki
 */

#include <stdlib.h>
#include <avr/io.h>
#include "Definitions.h"
#include "FastTransfer/ft.h"
#include "FastTransfer.h"
#include "Timer.h"
#include "LEDs.h"
#include "CommsDefenition.h"
#include "CANFastTransfer.h"
#include "Config.h"
#include "commsReceive.h"
#include "PeripheralSystems.h"
#include "MacroCommands.h"
#include "AT90CAN_UART.h"

//void clearWatchdog(void);
//Macro storage variables
int macroCommand=0, macroSubCommand=0;
enum {
	Idle,
	Pending,
	Active
} MACROstates;
unsigned char MacroStatus = Idle;

#define MACRO_COMMAND      FT_Read(&Control_ft_handle,MACROCOMMAND)
#define MACRO_SUB_COMMAND  FT_Read(&Control_ft_handle,MACROSUBCOMMAND);

//Command values stored locally
int leftMotorCommand= 0, rightMotorCommand=0, armMotorCommand=0, actuatorSpeed=0, plowMotorCommand=0;
timer_t pingTimer,TransmitManual;
timer_t macroResubmitTimer;
timer_t safetyTimer,commsTimer,commsTimerBeacon,checkCANTimer;
//Flag that says we are ready to send back to the control box
bool readyToSend=false;
void updateCANFTcoms();
void clearMotorCommandVars();
bool checkE_Stop();
void System_STOP();
void sendMacroCommand();
void sendManualCommand();

//Previous state holders, know when to process updates
int prevLeftMotorCommand, prevRightMotorCommand, prevBucketCommand, prevArmCommand;
int previousMacroCommand = STOP_MACRO, previousMacroSubCommand = STOP_MACRO;


bool macroCompletedLongAgo= true;
bool MacroModified;

typedef enum {
	pingControlBox=0,
	UpdateMacro,
	UpdateComs,
	UpdateManual
} ControlStates;
char COM_State = UpdateComs;

FT_t  Control_ft_handle;
#define  MACRO_FULL 3
#define  MACRO_PEND 2
#define  Connected 1
#define  Disconnected 0

char LEDstatus = Disconnected;
void initCOMs()
{
	// Initializing fast transfer for the coms between Controlbox and robot
	FT_Init(&Control_ft_handle,ROUTER_ADDRESS, USART1_put_C, USART1_get_C, isUART1_ReceiveEmpty);

}


timer_t upTimeCounter;
unsigned int pendingMacroIndex = 0;
unsigned int macroSubmitCount = 0;

void CommunicationsHandle() {
	// if any bytes of date have been received from the control box
	if(timerDone(&safetyTimer))//FT_Receive(&Control_ft_handle))
	{
		// Restart the timer since we received data
		resetTimer(&safetyTimer);
		// Parse out the FT data into local variables
		parseComms();
		// Check to see if a controlled stop was requested
		checkE_Stop();
		// This mask will control which devices are expected to have a good system status before continuing
		uint16_t mask = (1 << MOTOR_ADDRESS);// | (1 << MASTER_ADDRESS) | (1 << GYRO_ADDRESS) ;
		// Verify system statuses
		if(isSystemReady(mask))
		{
			printf("System (Good)\n");
			_delay_ms(2);
			// Is the current macro running not matching with the pending macro
			if(pendingMacroIndex != STOP && pendingMacroIndex & getCurrentMacro() == 0) {
				// Only need to send the macro request once in a while
				if(timerDone(&macroResubmitTimer))
				{
					// Keep track of how many times we have submitted so we can back out if we have tried too many times
					if(macroSubmitCount++ > 5)
					{
						pendingMacroIndex = STOP;
					}
				}
			}
			else if(FT_Modified(&Control_ft_handle,MACRO_COMMAND_INDEX) && macroCommand == STOP_MACRO)
			{
				// Update Macro System (pending values only given here)
				pendingMacroIndex = FT_Read(&Control_ft_handle,MACRO_COMMAND_INDEX);
			}
			else if(!FT_Modified(&Control_ft_handle,MACRO_COMMAND_INDEX) && macroCommand == STOP_MACRO)
			{
				// if there hasn't been a macro request then we can send a macro response to the control box of the STOP_MACRO
				FT_ToSend(&Control_ft_handle, MACRO_COMMAND_INDEX, STOP_MACRO);
				// Send the manual commands for the corresponding controller to handle them
				sendManualCommand();
				//TODO: Timer interval needed for sending information
			}
		}
		else
		{
			printf("System (Error)\n");
			// Clearing status flag to Reject Macros and send system status
			FT_Modified (&Control_ft_handle, MACRO_COMMAND_INDEX);
			FT_ToSend(&Control_ft_handle, MACRO_COMMAND_INDEX, STOP_MACRO);
		}
		//Reply to the Control Box with information (Macro status(ONLY if active), UP time)
		FT_ToSend(&Control_ft_handle, UPTIME_COUNTER_INDEX, getTimeElepsed(&upTimeCounter)/1000);
		// Reply to the Control Box with information (Macro status(ONLY if active), UP time)
		FT_Send(&Control_ft_handle, CONTROL_BOX_ADDRESS);

	}
	// If there hasn't been a received message from the control box in some amount of time we assume system is
	// not connected
	if(timerDone(&safetyTimer))
	{
		resetTimer(&upTimeCounter);
		//System_STOP();
	}


}


bool checkE_Stop() {
	if(FT_Modified(&Control_ft_handle,MACRO_COMMAND_INDEX) && FT_Read(&Control_ft_handle, MACRO_COMMAND) == STOP_MACRO) {
		System_STOP();
		return true;
	}
	return false;
}

void System_STOP()
{
	MacroStatus = Idle;
	macroCommand = STOP_MACRO;
	FT_ToSend(&Control_ft_handle, 1, STOP_MACRO);
	// send the packet
	FT_Send(&Control_ft_handle, CONTROL_BOX_ADDRESS);

	getCANFT_RFlag(CAN_COMMAND_INDEX);    /* Clear the Flag */
	/* Tell the Master to STOP */
	ToSendCAN(0, ROUTER_ADDRESS);
	ToSendCAN(CAN_COMMAND_INDEX, STOP_MACRO);
	ToSendCAN(CAN_COMMAND_DATA_INDEX, STOP_MACRO);    /* We don't need to send this Index but why not */
	sendDataCAN(MASTER_ADDRESS);
	/* Clearing an index CAN_FT receive Array So that there isn't an accident
	   and we jump back into a macro (not Likely but covering my ass) */
	setCANFTdata(CAN_COMMAND_INDEX,0,false);

}

void updateCANFTcoms()
{
	if (timerDone(&checkCANTimer))
	{
		if(ReceiveDataCAN())
		{


		}
	}
}
void sendMacroCommand()
{
	// Loading the CAN FastTransfer buffer with macro data
	ToSendCAN(MACRO_COMMAND_INDEX, FT_Read(&Control_ft_handle, MACRO_COMMAND_INDEX));
	ToSendCAN(CAN_COMMAND_DATA_INDEX,FT_Read(&Control_ft_handle, CAN_COMMAND_DATA_INDEX));
	// Sending.... the data on the CAN bus to the Master Controller for processing
	sendDataCAN(MASTER_ADDRESS);
}
void sendManualCommand()
{
	ToSendCAN(LEFTMOTORSPEED,leftMotorCommand);
	ToSendCAN(RIGHTMOTORSPEED,rightMotorCommand);
	ToSendCAN(ACTUATORSPEED,actuatorSpeed);
	ToSendCAN(ARMSPEED,armMotorCommand);
	ToSendCAN(PLOWSPEED,plowMotorCommand);
	sendDataCAN(MOTOR_ADDRESS);

}

void clearMotorCommandVars()
{
	int * receiveArrayAdd = getReceiveArray0();

	leftMotorCommand    =0;
	rightMotorCommand   =0;

	actuatorSpeed  =0;
	armMotorCommand     =0;
	plowMotorCommand    =0;

	receiveArrayAdd[LEFTMOTORSPEED] = 0;
	receiveArrayAdd[RIGHTMOTORSPEED] = 0;

	receiveArrayAdd[ACTUATORSPEED] = 0;
	receiveArrayAdd[ARMSPEED]=0;
	receiveArrayAdd[PLOWSPEED]=0;



}
void parseComms(void)
{


#ifndef REVERSE_LEFT_RIGHT
	leftMotorCommand    =(int)FT_Read(&Control_ft_handle, LEFTMOTORSPEED);
	rightMotorCommand   =(int)FT_Read(&Control_ft_handle, RIGHTMOTORSPEED);
#else
	leftMotorCommand    =(int)FT_Read(&Control_ft_handle, RIGHTMOTORSPEED);
	rightMotorCommand   =(int)FT_Read(&Control_ft_handle, LEFTMOTORSPEED);
#endif

	actuatorSpeed  =   FT_Read(&Control_ft_handle, ACTUATORSPEED);
	armMotorCommand     =   FT_Read(&Control_ft_handle, ARMSPEED);
	plowMotorCommand    = FT_Read(&Control_ft_handle,PLOWSPEED);

	//macroCommand        = FT_Read(&Control_ft_handle,MACROCOMMAND);
	//macroSubCommand     = FT_Read(&Control_ft_handle,MACROSUBCOMMAND);

}

bool manualMode(void)
{
	return (macroCommand==0);
}

void setupCommsTimers(void)
{
	setTimerInterval(&macroResubmitTimer,500);
	setTimerInterval(&pingTimer,500);
	setTimerInterval(&commsTimer,5);
	setTimerInterval(&commsTimerBeacon,5);
	setTimerInterval(&safetyTimer,2000);
	setTimerInterval(&checkCANTimer,50);
	setTimerInterval(&TransmitManual, 50);
}

int getMacroCommand(void)
{
	return macroCommand;
}

int getMacroSubCommand(void)
{
	return macroSubCommand;
}

void setMacroCommand(int m)
{
	macroCommand=m;

}

void setMacroSubCommand(int ms)
{
	macroSubCommand=ms;
}
