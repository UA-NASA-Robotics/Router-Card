/*
 * commsReceive.c
 *
 * Created: 4/26/2017 2:22:24 PM
 *  Author: Seth Carpenter
 *
 *
 * System wiki : https://github.com/UA-NASA-Robotics/ROCKEE_Router-Card/wiki
 */
#include "FastTransfer_CAN.h"
#include <stdlib.h>
//#include <avr/io.h>
//#include "Definitions.h"
#include "CommsIDs.h"
//#include "FastTransfer.h"
#include "timers.h"
//#include "LEDs.h"
#include "CommsDefenition.h"

#include "commsReceive.h"
#include "PeripheralSystems.h"

#include "uart1_config.h"
#include "uart2_config.h"

#define STOP 0


//#define DEBUG

//void clearWatchdog(void);
//Macro storage variables

FT_t  Control_ft_handle;

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


#define  MACRO_FULL 3
#define  MACRO_PEND 2
#define  Connected 1
#define  Disconnected 0

char LEDstatus = Disconnected;
void initCOMs()
{
	// Initializing fast transfer for the coms between Controlbox and robot
	FT_Init(&Control_ft_handle, ROUTER_CARD, uart1_put_c, uart1_get, uart1_rx_empty);
	
}


timer_t upTimeCounter;
unsigned int pendingMacroIndex = 0;
unsigned int macroSubmitCount = 0;

void CommunicationsHandle() {
	// if any bytes of date have been received from the control box
	if(FT_Receive(&Control_ft_handle))
	{


		// Check to see if a controlled stop was requested

		if(!checkE_Stop()) {
			// This mask will control which devices are expected to have a good system status before continuing
			uint16_t mask = 0;//(1 << MOTOR_CONTROLLER);// | (1 << MASTER_CONTROLLER) | (1 << GYRO_ADDRESS) ;
			// Verify system statuses
			if(isSystemReady(mask))
			{
				macroCommand = getCurrentMacro();

				// Is the current macro running not matching with the pending macro
				if(pendingMacroIndex != STOP_MACRO) {
					if((pendingMacroIndex & macroCommand) != pendingMacroIndex) {

						// Only need to send the macro request once in a while
						if(timerDone(&macroResubmitTimer, 1))
						{
							/* Keep track of how many times we have submitted so
							we can back out if we have tried too many times */
							if(macroSubmitCount++ > 5)
							{
								pendingMacroIndex = STOP;
								// this fakes the control box in thinking there was a macro before it clears all the macros
								FT_ToSend(&Control_ft_handle, MACRO_COMMAND_INDEX, pendingMacroIndex);

							}
							/* transmit macro on CAN bus */
							sendMacroCommand();
#ifdef DEBUG
							printf("transmitting Macro\n");
#endif
						}
					} else
					{
						pendingMacroIndex = 0;
					}
				}
				else if(FT_Modified(&Control_ft_handle,MACRO_COMMAND_INDEX))
				{
#ifdef DEBUG
					printf("macro Received: %d\n",FT_Read(&Control_ft_handle,MACRO_COMMAND_INDEX));
#endif
					FT_ClearFlag(&Control_ft_handle,MACRO_COMMAND_INDEX);
					// Update Macro System (pending values only given here)
					if((macroCommand | (FT_Read(&Control_ft_handle,MACRO_COMMAND_INDEX))) != macroCommand)
						pendingMacroIndex = FT_Read(&Control_ft_handle,MACRO_COMMAND_INDEX);

				}
				else if(!FT_Modified(&Control_ft_handle,MACRO_COMMAND_INDEX) && macroCommand == STOP_MACRO)
				{

//          if there hasn't been a macro request then we can send a macro response to the control box of the STOP_MACRO
//                    FT_ToSend(&Control_ft_handle, MACRO_COMMAND_INDEX, STOP_MACRO);
					// Send the manual commands for the corresponding controller to handle them
					macroSubmitCount = 0;
					//TODO: Timer interval needed for sending information
					if(timerDone(&TransmitManual, 1)) {
						sendManualCommand();
#ifdef DEBUG
						//printf("Manual....\n");
#endif
					}
				}
			}
			else
			{
#ifdef DEBUG
				printf("System (Error)\n");
#endif
				// Clearing status flag to Reject Macros and send system status
				FT_ClearFlag(&Control_ft_handle, MACRO_COMMAND_INDEX);
				FT_ToSend(&Control_ft_handle, MACRO_COMMAND_INDEX, STOP_MACRO);

			}
			//SetNeoPixRGB(LEDSTRIP_CONNECTED);
		}
		macroCommand = getCurrentMacro();
		//Reply to the Control Box with information (Macro status(ONLY if active), UP time)
		FT_ToSend(&Control_ft_handle, UPTIME_COUNTER_INDEX, getTimeElapsed(&upTimeCounter)/1000);

		FT_ToSend(&Control_ft_handle, MACRO_COMMAND_INDEX, macroCommand);
#ifdef DEBUG
//    printf("Uptime: %d\n",getTimeElapsed(&upTimeCounter)/1000);
//    printf("Macro: %d\n",macroCommand);
//    printf("pending: %d\n",pendingMacroIndex);
#endif
		// Reply to the Control Box with information (Macro status(ONLY if active), UP time)
		FT_Send(&Control_ft_handle, CONTROL_BOX);
		// Restart the timer since we received data
		ResetTimer(&safetyTimer);
		//getSystemLocData();



	}
	// If there hasn't been a received message from the control box in some amount of time we assume system is
	// not connected
	if(timerDone(&safetyTimer, 1))
	{
#if DEBUG
		printf("Not Connected Error\r");

#endif
		ResetTimer(&upTimeCounter);
		System_STOP();
		//flashLedColors(LEDSTRIP_CONNECT_PENDING_1,LEDSTRIP_CONNECT_PENDING_2);
	}


}


bool checkE_Stop() {
	if(FT_Modified(&Control_ft_handle,MACRO_COMMAND_INDEX) && FT_Read(&Control_ft_handle, MACRO_COMMAND_INDEX) == STOP_MACRO) {
		FT_ClearFlag(&Control_ft_handle,MACRO_COMMAND_INDEX);
		System_STOP();
		pendingMacroIndex =  STOP_MACRO;
		//printf("E_STOP!!\n");
		return true;
	}
	return false;
}

void System_STOP()
{
	// Loading the CAN FastTransfer buffer with macro data
	ToSendCAN(getGBL_MACRO_INDEX(ROUTER_CARD), STOP_MACRO);
	ToSendCAN(getGBL_MACRO_INDEX(ROUTER_CARD)+1,0);
	// Sending.... the data on the Global CAN bus to the for processing
	sendDataCAN(GLOBAL_ADDRESS);


	ToSendCAN(DRIVE_MOTOR_SPEED,0);
	ToSendCAN(ACTUATORSPEED,0);
	ToSendCAN(ARMSPEED,0);
	ToSendCAN(PLOWSPEED,0);
	sendDataCAN(MOTOR_CONTROLLER);

}

void sendMacroCommand()
{
	// Loading the CAN FastTransfer buffer with macro data
	ToSendCAN(getGBL_MACRO_INDEX(ROUTER_CARD), FT_Read(&Control_ft_handle, MACRO_COMMAND_INDEX));
	ToSendCAN(getGBL_MACRO_INDEX(ROUTER_CARD)+1,FT_Read(&Control_ft_handle, CAN_COMMAND_DATA_INDEX));
	//printf("MData: %d\n", FT_Read(&Control_ft_handle, CAN_COMMAND_DATA_INDEX));
	// Sending.... the data on the Global CAN bus to the for processing
	sendDataCAN(GLOBAL_ADDRESS);
}
void sendManualCommand()
{
	ToSendCAN(DRIVE_MOTOR_SPEED,FT_Read(&Control_ft_handle, DRIVE_MOTOR_SPEED));
	//ToSendCAN(RIGHTMOTORSPEED,rightMotorCommand);
	ToSendCAN(ACTUATORSPEED,FT_Read(&Control_ft_handle, ACTUATORSPEED));
	ToSendCAN(ARMSPEED,FT_Read(&Control_ft_handle, ARMSPEED));
	//printf("Bucket: %d\n",FT_Read(&Control_ft_handle, ACTUATORSPEED));
	ToSendCAN(PLOWSPEED,FT_Read(&Control_ft_handle, PLOWSPEED));
	sendDataCAN(MOTOR_CONTROLLER);
	//printf("DriveSpeed: %d\n",(signed char)FT_Read(&Control_ft_handle, DRIVE_MOTOR_SPEED));

}

void clearMotorCommandVars()
{
	//int * receiveArrayAdd = getReceiveArray0();

    uint16_t* receiveArrayAdd = Control_ft_handle.array;
    
	leftMotorCommand    =0;
	rightMotorCommand   =0;

	actuatorSpeed  =0;
	armMotorCommand     =0;
	plowMotorCommand    =0;

	receiveArrayAdd[DRIVE_MOTOR_SPEED] = 0;
	receiveArrayAdd[ACTUATORSPEED] = 0;
	receiveArrayAdd[ARMSPEED]=0;
	receiveArrayAdd[PLOWSPEED]=0;



}
void parseComms(void)
{


// #ifndef REVERSE_LEFT_RIGHT
//  leftMotorCommand    =(int)FT_Read(&Control_ft_handle, DRIVE_MOTOR_SPEED);
//  rightMotorCommand   =(int)FT_Read(&Control_ft_handle, RIGHTMOTORSPEED);
// #else
//  leftMotorCommand    =(int)FT_Read(&Control_ft_handle, RIGHTMOTORSPEED);
//  rightMotorCommand   =(int)FT_Read(&Control_ft_handle, LEFTMOTORSPEED);
// #endif

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
