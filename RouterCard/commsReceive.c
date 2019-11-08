/*
 * commsReceive.c
 *
 * Created: 4/26/2017 2:22:24 PM
 *  Author: Zac
 */

#include <stdlib.h>
#include <avr/io.h>
#include "Definitions.h"
#include "FastTransfer/ft.h"
#include "FastTransfer.h"
#include "Timer.h"
#include "LEDs.h"
#include "MotorSystems/Motor.h"
#include "CommsDefenition.h"
#include "CANFastTransfer.h"
#include "Config.h"
#include "commsReceive.h"
#include "PeripheralSystems.h"

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

void updateComs2(void)
{
	FT_Receive(&Control_ft_handle);
	toggleLED(7);
	switch(COM_State)
	{
	case pingControlBox:

		FT_Receive(&Control_ft_handle);
		if(timerDone(&pingTimer))
		{
			if(MacroStatus == Active)
			{
				FT_ToSend(&Control_ft_handle, SENDER_ADDRESS_INDEX, ROUTER_ADDRESS);

				FT_ToSend(&Control_ft_handle, CAN_COMMAND_INDEX, macroCommand);

				// send the packet
				FT_Send(&Control_ft_handle, ControlBoxAddressFT);

			} else {
				FT_ToSend(&Control_ft_handle, SENDER_ADDRESS_INDEX, ROUTER_ADDRESS);
				//FT_ToSend(&Control_ft_handle, 50, getCANFTdata(3));      //This should be the force Sensor data
				// send the packet
				FT_Send(&Control_ft_handle, ControlBoxAddressFT);

			}


		}
		COM_State = UpdateManual;
		break;
	case UpdateMacro:
		//FT_Receive(&Control_ft_handle);
		MacroModified = FT_Modified(&Control_ft_handle,MACRO_COMMAND_INDEX);
		/* If we have a new macro */
		if(MacroModified == true && MACRO_COMMAND != STOP_MACRO)// && (timerDone(&macroResubmitTimer) || macroCompletedLongAgo) )//|| (previousMacroSubCommand!=macroSubCommand))
		{
			//toggleLED(11);
			MacroStatus = Pending;
			macroCommand = MACRO_COMMAND;
			previousMacroCommand = macroCommand;


		}
		/* If we Have a new entry for the stop command */
		else if(MACRO_COMMAND == STOP_MACRO && MacroModified)
		{
			toggleLED(12);
			System_STOP();
			MacroStatus = Idle;
			macroCommand = MACRO_COMMAND;
			previousMacroCommand = STOP_MACRO;
			SetNeoPixRGB(LEDSTRIP_CONNECTED);

		}
		//TODO: if There is a 0 in macro index send all peripherals on the can bus a stop
		if(MacroStatus == Active)
		{
			LEDstatus = MACRO_PEND;
			if(getCANFT_RFlag(CAN_COMMAND_INDEX) == true && getCANFTdata(SENDER_ADDRESS_INDEX) == MasterAddress) {
				if(LEDstatus !=MACRO_FULL)
					SetNeoPixRGB(LEDSTRIP_MACRO_RUNNING);
				if(getCANFTdata(CAN_COMMAND_INDEX) == STOP_MACRO) {
					MacroStatus = Idle;

					FT_ToSend(&Control_ft_handle, 1, STOP_MACRO);
					// send the packet
					FT_Send(&Control_ft_handle, ControlBoxAddressFT);
					setLED(11,OFF);
					SetNeoPixRGB(LEDSTRIP_CONNECTED);
				}
			}
			setLED(11,ON);
			COM_State = UpdateComs;
		} else if(MacroStatus == Pending) {
			/* Has the Master Responded */
			toggleLED(2);

			flashLedColors(LEDSTRIP_CONNECT_PENDING_1,LEDSTRIP_CONNECT_PENDING_2);
			if(getCANFT_RFlag(CAN_COMMAND_INDEX) == true && getCANFTdata(SENDER_ADDRESS_INDEX) == MasterAddress) {

				if(getCANFTdata(CAN_COMMAND_INDEX) == macroCommand) {

					FT_ToSend(&Control_ft_handle, 1, macroCommand);
					// send the packet
					FT_Send(&Control_ft_handle, ControlBoxAddressFT);
					MacroStatus = Active;

				}

			}
			if(timerDone(&macroResubmitTimer))
			{
				toggleLED(6);
				ToSendCAN(0, RouterCardAddress);
				ToSendCAN(CAN_COMMAND_INDEX, macroCommand);
				ToSendCAN(CAN_COMMAND_DATA_INDEX, macroSubCommand);
				sendDataCAN(MasterAddress);
			}
			COM_State = UpdateComs;
		} else {
			setLED(11,OFF);
			//COM_State = UpdateManual;
		}
		COM_State = UpdateComs;

		break;
	case UpdateComs:
		FT_Receive(&Control_ft_handle);
		if(timerDone(&commsTimer)) {
			ReceiveDataCAN();

			toggleLED(3);
			if(FT_Modified(&Control_ft_handle,0) || FT_Modified(&Control_ft_handle,1)) {
				resetTimer(&safetyTimer);
				setLED(5,OFF);
				//toggleLED(2);
			}
			parseComms();
			//If we haven't heard from the ControlBox stop motors and macros and clear values
			if(!timerDone_NoReset(&safetyTimer)) {
				COM_State = pingControlBox;
				toggleLED(4);
				if((MacroStatus != Active) && (MacroStatus != Pending) && LEDstatus == Disconnected) {
					SetNeoPixRGB(LEDSTRIP_CONNECTED);
					LEDstatus = Connected;
				}

			} else {
				flashLedColors(LEDSTRIP_CONNECT_PENDING_1,LEDSTRIP_CONNECT_PENDING_2);
				LEDstatus = Disconnected;
				if(timerDone(&macroResubmitTimer)) {
					System_STOP();
					macroCommand = STOP_MACRO;
					MacroStatus = Idle;
					toggleLED(5);
				}
			}
		}
		break;
	case UpdateManual:
		//FT_Receive(&Control_ft_handle);
		if(!timerDone_NoReset(&safetyTimer) && timerDone(&TransmitManual)&& MacroStatus == Idle) {
			motorControl(leftMotorCommand, rightMotorCommand, actuatorSpeed, armMotorCommand,plowMotorCommand);
		}
		COM_State = UpdateMacro;
		break;
	}

}
timer_t upTimeCounter;
unsigned int pendingMacroIndex = 0;
unsigned int macroSubmitCount = 0;
void CommunicationsHandle()
{
	if(ReceiveDataCAN())
	{
		updateCANFTcoms();
		// TODO: Update global information
		// Macro Confirmations (Running values given)

	}
	// if any bytes of date have been received from the control box
	if(FT_Receive(&Control_ft_handle))
	{
		// Parse out the FT data into local variables
		parseComms();
		// Check to see if a controlled stop was requested
		checkE_Stop();
		// Verify system statuses
		if(isSystemReady())
		{
			// Was Macro information received
			if(pendingMacroIndex != STOP) {
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
			else if(FT_Modified(&Control_ft_handle,MACRO_COMMAND_INDEX) && macroCommand == STOP_MACRO) {
				// Update Macro System (pending values only given here)
				pendingMacroIndex = FT_Read(&Control_ft_handle,MACRO_COMMAND_INDEX);
			}
			else if(!FT_Modified(&Control_ft_handle,MACRO_COMMAND_INDEX) && macroCommand == STOP_MACRO)
			{
				// if there hasn't been a macro request then we can send a macro response to the control box of the STOP_MACRO
				FT_ToSend(&Control_ft_handle, MACRO_COMMAND_INDEX, STOP_MACRO);
				// Send the manual commands for the corresponding controller to handle them
				sendManualCommand();
			}
		} else {
			// Clearing status flag to Reject Macros and send system status
			FT_Modified (&Control_ft_handle, MACRO_COMMAND_INDEX);
			FT_ToSend(&Control_ft_handle, MACRO_COMMAND_INDEX, STOP_MACRO);
		}
		//Reply to the Control Box with information (Macro status(ONLY if active), UP time)
		FT_ToSend(&Control_ft_handle, UPTIME_COUNTER_INDEX, getTimeElepsed(&upTimeCounter));
		// Reply to the Control Box with information (Macro status(ONLY if active), UP time)
		FT_Send(&Control_ft_handle, ControlBoxAddressFT);

	}
	// If there hasn't been a received message from the control box in some amount of time we assume system is
	// not connected
	if(timerDone(&safetyTimer))
	{
		resetTimer(&upTimeCounter);
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
	FT_Send(&Control_ft_handle, ControlBoxAddressFT);

	getCANFT_RFlag(CAN_COMMAND_INDEX);    /* Clear the Flag */
	/* Tell the Master to STOP */
	ToSendCAN(0, RouterCardAddress);
	ToSendCAN(CAN_COMMAND_INDEX, STOP_MACRO);
	ToSendCAN(CAN_COMMAND_DATA_INDEX, STOP_MACRO);    /* We don't need to send this Index but why not */
	sendDataCAN(MasterAddress);
	/* Clearing an index CAN_FT receive Array So that there isn't an accident
	   and we jump back into a macro (not Likely but covering my ass) */
	setCANFTdata(CAN_COMMAND_INDEX,0);
	/* SHUT THE MOTORS UP */
	motorControl(0, 0, 0, 0, 0);
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
	sendDataCAN(MasterAddress);
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

int getLeftMotorCommand(void)
{
	return leftMotorCommand;
}

int getRightMotorCommand(void)
{
	return rightMotorCommand;
}

int getArmMotorCommand(void)
{
	return armMotorCommand;
}

int getBucketMotorCommand(void)
{
	return actuatorSpeed;
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
