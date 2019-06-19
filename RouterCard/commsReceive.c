/*
 * commsReceive.c
 *
 * Created: 4/26/2017 2:22:24 PM
 *  Author: Zac
 */ 

#include "commsReceive.h"
#include <stdlib.h>
#include <avr/io.h>
#include "FastTransfer.h"
#include "Timer.h"
#include "LEDs.h"
#include "MotorSystems/Motor.h"
#include "CommsDefenition.h"
#include "CANFastTransfer.h"
#include "Config.h"
<<<<<<< HEAD

=======
#include "..\..\..\Common\CommsDefenition.h"
>>>>>>> develop

//void clearWatchdog(void);
//Macro storage variables
int macroCommand=0, macroSubCommand=0;
bool macroActive = false;

//Command values stored locally
int leftMotorCommand, rightMotorCommand, armMotorCommand, bucketMotorCommand;

//Timers for delay control
timer_t sendBackTimer, commsTimer, safetyTimer, mouseGyroTimer;
timer_t commsTimerBeacon, beaconPollTimer;
timer_t resendMotorTimerLeft,resendMotorTimerRight,resendMotorTimerBucket;

//Flag that says we are ready to send back to the control box
bool readyToSend=false;

//Previous state holders, know when to process updates
int prevLeftMotorCommand, prevRightMotorCommand, prevBucketCommand, prevArmCommand;
int previousMacroCommand, previousMacroSubCommand;

//Data from the beacon
int lidarAngle, lidarDistance, lidarDisplacement, lidarHeading;


void updateComms(void)
{
	//updateCommsBeacon();
	
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
		if((previousMacroCommand!=macroCommand) || (previousMacroSubCommand!=macroSubCommand))
		{
// 			//Send to the sensor/navi an update of the macro state
// 			ToSendCAN(0, RouterCardAddress);
// 			ToSendCAN(1, getMacroCommand());
// 			ToSendCAN(2, getMacroSubCommand());
// 			sendDataCAN(SensorAddress);
// 				
// 			//Mark that we have updated them
// 			previousMacroSubCommand=macroSubCommand;
// 			previousMacroCommand=macroCommand;			
		}
	}
		
	if(timerDone(&commsTimer)) //Check the comms when the timer says to
	{
		bool gotData=false;
		
		//If there is a new incoming data packet from the control box
		while(receiveData1())	//PULL IT ALL OUT	
		{
			PORTA^=(0x80); //Toggle A7 when received
			gotData=true;
		}
		
		//ONLY PROCESS DATA IF THE RECEIVE LOOP GOT DATA
		if(gotData)
		{
			//Look through the packet and sort incoming data
			parseComms();
				
			#ifdef NEW_MAIN
				//Debug LEDs
				toggleLED(LED12);
			#endif

			//If we are currently in manual drive mode then send motor commands
			if(manualMode())
			{
				
				setLED(4,OFF);
				//Mark that we are not active in an macro 
				macroActive = false;
				
				//Indicators
				if(abs(getRightMotorCommand())>12)
				{
					//printf("R Speed: %d\r\n", getRightMotorCommand());
					setLED(11,ON);
				}
				else
				{
					setLED(11,OFF);
					rightMotorCommand=(0);
				}
					
				//Indicators
				if(abs(getLeftMotorCommand())>12)
				{
					//printf("L Speed: %d\r\n", getLeftMotorCommand());
					setLED(10,ON);
				}
				else
				{
						setLED(10,OFF);
						leftMotorCommand=(0);
				}
				//Indicators
				if(abs(getBucketMotorCommand())>12)
				{
					//printf("L Speed: %d\r\n", getLeftMotorCommand());
					setLED(12,ON);
				}
				else
				{
					setLED(12,OFF);
					leftMotorCommand=(0);
				}
					
				#ifndef DISABLE_MOTOR_SYSTEMS
				
					//*********************************RIGHT MOTOR UPDATE**********************************					
					#ifndef DISABLE_RIGHT_MOTOR
						//If the current motor command is not the same as the last sent
						if(prevRightMotorCommand!=getRightMotorCommand()|| timerDone(&resendMotorTimerRight))
						{
							//Send the right motor speed control
	 						RightMotor_SetVelNoCommsSafety(40*getRightMotorCommand());
	 						
							//Store value previously sent
							prevRightMotorCommand=getRightMotorCommand();
						}
					#endif /* DISABLE_RIGHT_MOTOR */
					
					
					//*********************************LEFT MOTOR UPDATE**********************************
					#ifndef DISABLE_LEFT_MOTOR
						if(prevLeftMotorCommand!=getLeftMotorCommand() || timerDone(&resendMotorTimerLeft))
						{
							//Send the left motor speed control
 							LeftMotor_SetVelNoCommsSafety(-40*getLeftMotorCommand()); //about 100 from control box *40 goes up to max speed of 4000RPM on drive motors.
							//Store value previously sent
							prevLeftMotorCommand=getLeftMotorCommand();
						}
					#endif /* DISABLE_LEFT_MOTOR */

					//*********************************ARM MOTOR UPDATE**********************************					
					#ifndef	DISABLE_ARM_MOTOR	
						//If the current motor command is not the same as the last sent
						if(prevArmCommand!=getArmMotorCommand())
						{
							//SET ARM MOTOR COMMANDS
							if(getArmMotorCommand())
							{
								ArmMotor_SetVelNoCommsSafety(4000);
							}
							else
							{
								ArmMotor_SetVel(0);
							}
							
							//Store value previously sent
							prevArmCommand=getArmMotorCommand();
						}
					#endif /* DISABLE_ARM_MOTOR */
					
					//*********************************BUCKET MOTOR UPDATE**********************************					
					#ifndef DISABLE_BUCKET_MOTOR
						//If the current motor command is not the same as the last sent
						if(prevBucketCommand!=getBucketMotorCommand()|| timerDone(&resendMotorTimerBucket))
						{
							//SET BUCKET MOTOR COMMANDS
							switch(getBucketMotorCommand()) //254 is up
							{
								case 254: //254 is up
									BucketMotor_SetVelNoCommsSafety(1000); //can be pushed to 3000 if faster dumping is needed.
									break;
								case 253: //253 is down
									BucketMotor_SetVelNoCommsSafety(-1000); // can be pushed to -3000 if faster dumping is needed.
									break;
								default:
									BucketMotor_SetVelNoCommsSafety(0);
							}
							
							//Store value previously sent
							prevBucketCommand=getBucketMotorCommand();
						}
					#endif /* DISABLE_BUCKET_MOTOR */
				#endif /* DISABLE_MOTOR_SYSTEMS */
			}
			else    //Else we are not in manual mode (meaning there is a macro present)
			{
				macroActive = true;
				
				#ifdef MACROS_RESET_ROUTER
					//IF the macro sent is the fully autonomous macro, then the watchdog is enabled, and forgotten... causing a system reboot on this end..
					if(macroCommand==7)
						WDTCR= (1<<3)| (0b00000111);		//Enable watchdog with prescaler 111
				#endif
				
				#ifdef NEW_MAIN
					toggleLED(LED2);
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
		#ifndef DISABLE_MOTOR_SYSTEMS
		
			#ifndef DISABLE_LEFT_MOTOR
				#ifdef LEFT_MOTOR_USING_CAN
					LeftMotor_SetVelNoCommsSafety(0);
				#else
					LeftMotorSendVel(0);
				#endif
			#endif
			
			#ifndef DISABLE_RIGHT_MOTOR
				#ifdef RIGHT_MOTOR_USING_CAN
					RightMotor_SetVelNoCommsSafety(0);
				#else
					RightMotorSendVel(0);
				#endif
			#endif
			
			#ifndef DISABLE_BUCKET_MOTOR
				BucketMotor_SetVelNoCommsSafety(0);
			#endif
			
			#ifndef DISABLE_ARM_MOTOR
				ArmMotor_SetVelNoCommsSafety(0);
			#endif
			
		#endif
			
		//Send a packet to the control box to reestablish communications
		#ifdef SEND_TO_CONTROL_FOR_COMMS_ESTABLISH
			ToSend1(0,4);
			ToSend1(1,macroCommand);
			sendData1(5);
		#endif
			
		//Indicator that we are in comms safety loop
		toggleLED(8);
		resetTimer(&safetyTimer);
	}
}


void updateCommsBeacon(void)
{
	if(timerDone(&beaconPollTimer))
	{
		ToSend0(LIDAR_COMMAND_INDEX,LIDAR_GET_LEAPFROG_HEADING);
		sendData0(BeaconAddress);
		resetTimer(&beaconPollTimer);
	}
	
	if(timerDone(&commsTimerBeacon)) //Check the comms when the timer says to
	{
		while(receiveData0())	//If there is a new incoming data packet from the control box
		{
			int * receiveArrayAdd	= getReceiveArray0();
			lidarAngle				= (int)receiveArrayAdd[LIDAR_OBJ_ANGLE];
			lidarDisplacement		= (int)receiveArrayAdd[LIDAR_OBJ_AtoB_DIS];
			lidarHeading			= receiveArrayAdd[LIDAR_OBJ_HEADING];
			lidarDistance		    = receiveArrayAdd[LIDAR_OBJ_MAG];	
		}
	}
}

void parseComms(void)
{
	int * receiveArrayAdd	=getReceiveArray1();
	#ifndef REVERSE_LEFT_RIGHT
		leftMotorCommand		=(int)receiveArrayAdd[LEFTMOTORSPEED];
		rightMotorCommand		=(int)receiveArrayAdd[RIGHTMOTORSPEED];
	#else
		leftMotorCommand		=(int)receiveArrayAdd[RIGHTMOTORSPEED];
		rightMotorCommand		=(int)receiveArrayAdd[LEFTMOTORSPEED];	
	#endif
	
	bucketMotorCommand		=receiveArrayAdd[ACTUATORSPEED];
	armMotorCommand	=receiveArrayAdd[ARMSPEED];

	macroCommand			=receiveArrayAdd[MACROCOMMAND];
	macroSubCommand			=receiveArrayAdd[MACROSUBCOMMAND];

	if(macroCommand!=0)
	{
		toggleLED(3);
		
		//Send this macro stuff to the control box processor via FastTransfer
		ToSend1(0,4);
		ToSend1(1, macroCommand);
		ToSend1(2,macroSubCommand);
		sendData1(ControlBoxAddressFT);
		
		testOpenLoopCommandsLeftAndRight(100000);
		macroCommand=0;
		macroSubCommand=0;
		
		//Send this macro stuff to the control box processor via FastTransfer
		ToSend1(0,4);
		ToSend1(1, macroCommand);
		ToSend1(2,macroSubCommand);
		sendData1(ControlBoxAddressFT);
	}
}

bool manualMode(void)
{
	return macroCommand==0;
}

void setupCommsTimers(void)
{
	setTimerInterval(&mouseGyroTimer,50);
	setTimerInterval(&commsTimer,2);
	setTimerInterval(&commsTimerBeacon,5);
	setTimerInterval(&beaconPollTimer,50);
	setTimerInterval(&sendBackTimer, 2);
	setTimerInterval(&safetyTimer,1000);
	setTimerInterval(&resendMotorTimerRight,50);
	setTimerInterval(&resendMotorTimerLeft,50);
	setTimerInterval(&resendMotorTimerBucket,50);
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
