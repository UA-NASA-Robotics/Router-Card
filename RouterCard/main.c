/*
 * RouterCard.c
 *
 * Created: 4/13/2016 11:28:41 PM
 * Author : reed
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "can.h"
#include "CommsDefenition.h"
#include "FastTransfer.h" 
#include "general.h"
#include "assert.h"
#include "MotorSystems/Motor.h" 
#include "CANFastTransfer.h"
#include "commsReceive.h"
#include "LEDs.h"
#include "Config.h"
#include "InputButtons.h"
#include "Timer.h"
#include "Init.h"
#define __AVR_CAN90CAN128 



//Time keeps for comms management
timer_t ledTimer,  checkCANTimer, motorRampTimer;
	 
	 
int main(void) 
{
	initialize();
	setTimerInterval(&motorRampTimer,100);
	
// 	testOpenLoopCommandsLeftAndRight(100000);
// 	testOpenLoopCommandsLeftAndRightTurn(-120000);
// 	testOpenLoopCommandsLeftAndRight(100000);
// 	testOpenLoopCommandsLeftAndRightTurn(-120000);
// 	testOpenLoopCommandsLeftAndRight(100000);
// 	testOpenLoopCommandsLeftAndRightTurn(-120000);
// 	testOpenLoopCommandsLeftAndRight(100000);
// 	testOpenLoopCommandsLeftAndRightTurn(-120000);
	
	while(1) 
	{
		
		//BLINK A LIGHT
		if(timerDone(&ledTimer))	
		{
			#ifdef NEW_MAIN
				toggleLED(6);	
			#else
				toggleLED(TEST1);
			#endif
		}
		
		
		
// 		if(timerDone(&motorRampTimer))
// 		{
// 			static int motorSpeedSend;
// 			static bool updown=true;
// 			if(updown && motorSpeedSend<100)
// 			{
// 				
// 				motorSpeedSend++;
// 				
// 			}
// 			else if(updown)
// 			{
// 				updown = false;
// 				
// 			}
// 			else if(!updown && motorSpeedSend >-100)
// 			{
// 				motorSpeedSend--;
// 			}
// 			else if(!updown)
// 			{
// 				updown=true;
// 			}
// 			
// 			RightMotor_SetVelNoCommsSafety(40*motorSpeedSend);
// 			LeftMotor_SetVelNoCommsSafety(40*motorSpeedSend);
// 			
// 		}
		//if (timerDone(&checkCANTimer)) 
		//{
			//if(ReceiveDataCAN())
			//{
				//
			//}
		//}
		
		//UPDATE THE COMMUNICATIONS		
		updateComms();
		
		 
	#ifdef USE_BUTTONS_TEST
		buttonsInputTest();
	#endif
	
	}
}