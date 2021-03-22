/*
 * commsReceive.c
 *
 * Created: 4/26/2017 2:22:24 PM
 *  Author: Seth
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
int leftMotorCommand= 0, rightMotorCommand=0, armMotorCommand=0, bucketMotorCommand=0, plowMotorCommand=0;
timer_t pingTimer,TransmitManual;
timer_t macroResubmitTimer,ohShitWeAreDeadTimer;
timer_t safetyTimer,commsTimer,commsTimerBeacon,checkCANTimer;
timer_t sendBackTimer, resendMotorTimerRight,resendMotorTimerLeft,resendMotorTimerBucket;
//Flag that says we are ready to send back to the control box
bool readyToSend=false;
void updateCANFTcoms();
void clearMotorCommandVars();
void System_STOP();
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
//FT_t * Beacon_ft_handle;
#define  MACRO_FULL 3
#define  MACRO_PEND 2
#define  Connected 1
#define  Disconnected 0

char LEDstatus = Disconnected;
void initCOMs()
{
	FT_Init(&Control_ft_handle,ROUTER_ADDRESS, USART1_put_C, USART1_get_C, isUART1_ReceiveEmpty);
	//Beacon_ft_handle = FT_Create( ROUTER_ADDRESS, USART0_put_C, USART0_get_C, isUART0_ReceiveEmpty);

}

void updateComs2(void)
{
	//while(1)
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
		if(MacroModified && MACRO_COMMAND != STOP_MACRO)// && (timerDone(&macroResubmitTimer) || macroCompletedLongAgo) )//|| (previousMacroSubCommand!=macroSubCommand))
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
			// MacroStatus = Idle;
			// macroCommand = MACRO_COMMAND;
			previousMacroCommand = STOP_MACRO;
			SetNeoPixRGB(LEDSTRIP_CONNECTED);

		}
		//TODO: if There is a 0 in macro index send all peripherals on the can bus a stop
		if(MacroStatus == Active)
		{
			LEDstatus = MACRO_PEND;
			if(getCANFT_RFlag(CAN_COMMAND_INDEX) && getCANFTdata(SENDER_ADDRESS_INDEX) == MasterAddress) {
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
			if(getCANFT_RFlag(CAN_COMMAND_INDEX) && getCANFTdata(SENDER_ADDRESS_INDEX) == MasterAddress) {

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
					// macroCommand = STOP_MACRO;
					// MacroStatus = Idle;
					toggleLED(5);
				}
			}
		}
		break;
	case UpdateManual:
		//FT_Receive(&Control_ft_handle);
		if(!timerDone_NoReset(&safetyTimer) && timerDone(&TransmitManual)&& MacroStatus == Idle) {
			motorControl(leftMotorCommand, rightMotorCommand, bucketMotorCommand, armMotorCommand,plowMotorCommand);
		}
		COM_State = UpdateMacro;
		break;
	}

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

void updateComms(void)
{
//  setTimerInterval(&commsTimer,200);
//  while(1)
//  {
//    if(timerDone(&commsTimer))
//    {
//
//      ToSend1(1, 0);
//      sendData1(ControlBoxAddress);
//    }
//  }

	updateCANFTcoms();
	//updateCommsBeacon(LIDAR_IDLE_STATE);

	//If we have heard from the control box and waited a short time before we are now going to send back
	if(readyToSend && timerDone(&sendBackTimer))
	{
		//Return comms to the control box
		ToSend1(0,4);
		ToSend1(1,macroCommand);
		ToSend1(2,macroSubCommand);
		sendData1(ControlBoxAddressFT);

		//Mark that we have responded (wait for next comms incoming)
		readyToSend=false;

		//If the macro has not been processed yet
		if((macroCommand!=0 && previousMacroCommand==0))// && (timerDone(&macroResubmitTimer) || macroCompletedLongAgo) )//|| (previousMacroSubCommand!=macroSubCommand))
		{
			toggleLED(4);
//      //Send to the sensor/navi an update of the macro state
			ToSendCAN(0, RouterCardAddress);
			ToSendCAN(CAN_COMMAND_INDEX, macroCommand);
			ToSendCAN(CAN_COMMAND_DATA_INDEX, macroSubCommand);
			sendDataCAN(MasterAddress);

			setCANFTdata(CAN_COMMAND_INDEX,1);


			//Mark that we have updated them
			previousMacroCommand=1;
		}
		else if (macroCommand==0 && previousMacroCommand!=0)
		{
			previousMacroCommand=0;
			ToSendCAN(0, RouterCardAddress);
			ToSendCAN(CAN_COMMAND_INDEX, macroCommand);
			ToSendCAN(CAN_COMMAND_DATA_INDEX, macroSubCommand);
			sendDataCAN(MasterAddress);

			setCANFTdata(CAN_COMMAND_INDEX,0);

		}
		else if(timerDone(&macroResubmitTimer) && !macroCompletedLongAgo)
		{

			macroCompletedLongAgo=true;
			macroCommand=0;
			macroSubCommand=0;
		}
	}

	if(timerDone(&commsTimer)) //Check the comms when the timer says to
	{
		//bool gotData=false;

		//If there is a new incoming data packet from the control box
		if(receiveData1())  //PULL IT ALL OUT
		{
			//PORTA^=(0x80); //Toggle A7 when received
			//gotData=true;


			resetTimer(&ohShitWeAreDeadTimer);
		

		//ONLY PROCESS DATA IF THE RECEIVE LOOP GOT DATA
		

			//Look through the packet and sort incoming data
			parseComms();

#ifdef NEW_MAIN
			//Debug LEDs
			toggleLED(LED12);
#endif

			//If we are currently in manual drive mode then send motor commands
			if(macroCommand==0)//manualMode())
			{
				//toggleLED(11);
//        if(abs(leftMotorCommand)>0)
//        {
//          setLED(11,1);
//        }
//        else
//        {
//          setLED(11,0);
//        }
				//toggleLED(8);
				//TODO: Call motor run
				motorControl(leftMotorCommand, rightMotorCommand, bucketMotorCommand, armMotorCommand,plowMotorCommand);
				//setLED(2,ON);

			}
			else    //Else we are not in manual mode (meaning there is a macro present)
			{
				MacroStatus = Active;

#ifdef MACROS_RESET_ROUTER
				//IF the macro sent is the fully autonomous macro, then the watchdog is enabled, and forgotten... causing a system reboot on this end..
				if(macroCommand==7)
					WDTCR= (1<<3)| (0b00000111);    //Enable watchdog with prescaler 111
#endif

#ifdef NEW_MAIN
				//setLED(LED2,OFF);
#endif
			}

			//Mark that we heard from the control, and we are ready to send back
			readyToSend=true;

			//Reset the safety timeout
			resetTimer(&safetyTimer);

			//Reset the timer to delay a short while before sending back
			resetTimer(&sendBackTimer);
		
		}
		//Reset the check comms timer
		resetTimer(&commsTimer);
	}

	if(timerDone(&safetyTimer))
	{
		//MotorsAllStop();
		clearMotorCommandVars();
		//Send a packet to the control box to reestablish communications
#ifdef SEND_TO_CONTROL_FOR_COMMS_ESTABLISH
		ToSend1(0,4);
		ToSend1(1,macroCommand);
		sendData1(ControlBoxAddress);
#endif

		//Indicator that we are in comms safety loop
		toggleLED(8);
		resetTimer(&safetyTimer);
		if(timerDone(&ohShitWeAreDeadTimer))
		{
			WDTCR= (1<<3)| (0b00000111);    //Enable watchdog with prescaler 111

		}
	}
	// else
	// {
	// }
}

void updateCANFTcoms()
{
	if (timerDone(&checkCANTimer))
	{
		if(ReceiveDataCAN())
		{
			toggleLED(LED4);
			//if(getCANFTdata(SENDER_ADDRESS_INDEX) == MasterAddress)
			//{
			//updateCommsBeacon(getCANFTdata(REQUEST_BEACON_DATA));
			clearMotorCommandVars(REQUEST_BEACON_DATA);
			clearMotorCommandVars(SENDER_ADDRESS_INDEX);
			//}
			if( macroCommand != 0  && (getCANFTdata(CAN_COMMAND_INDEX) == 0))// && (timerDone(&macroResubmitTimer) || macroCompletedLongAgo))
			{
				setLED(3,OFF);
				//wipeRxBuffer1();

				ToSend1(1, 0);
				ToSend1(2, 0);
				sendData1(ControlBoxAddress);
				macroCommand=0;
				macroSubCommand=0;


				//readyToSend = TRUE;
				_delay_ms(50);
				//USART1_Flush();
				ToSend1(1, 0);
				sendData1(ControlBoxAddress);

				resetTimer(&macroResubmitTimer);
				macroCompletedLongAgo=false;


				//_delay_ms(50);
				//ToSend1(1, 0);
				//sendData1(ControlBoxAddress);
				//while(1);
			}

		}
	}
}


void clearMotorCommandVars()
{
	int * receiveArrayAdd = getReceiveArray0();

	leftMotorCommand    =0;
	rightMotorCommand   =0;

	bucketMotorCommand    =0;
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
	//if(&Control_ft_handle != NULL)


#ifndef REVERSE_LEFT_RIGHT
	leftMotorCommand    =(int)FT_Read(&Control_ft_handle, LEFTMOTORSPEED);
	rightMotorCommand   =(int)FT_Read(&Control_ft_handle, RIGHTMOTORSPEED);
#else
	leftMotorCommand    =(int)FT_Read(&Control_ft_handle, RIGHTMOTORSPEED);
	rightMotorCommand   =(int)FT_Read(&Control_ft_handle, LEFTMOTORSPEED);
#endif

	bucketMotorCommand  =   FT_Read(&Control_ft_handle, ACTUATORSPEED);
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
	setTimerInterval(&ohShitWeAreDeadTimer,10000);
	setTimerInterval(&macroResubmitTimer,500);
	setTimerInterval(&pingTimer,500);
	setTimerInterval(&commsTimer,5);
	setTimerInterval(&commsTimerBeacon,5);
	setTimerInterval(&sendBackTimer, 1);
	setTimerInterval(&safetyTimer,2000);
	setTimerInterval(&resendMotorTimerRight,50);
	setTimerInterval(&resendMotorTimerLeft,50);
	setTimerInterval(&resendMotorTimerBucket,50);
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
	return bucketMotorCommand;
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
