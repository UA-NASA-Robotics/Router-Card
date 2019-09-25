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
timer_t ledTimer,  checkCANTimer, motorRampTimer,watchDog;

bool flipFlop= true;

int main(void)
{

	initialize();

	initCOMs();


	setTimerInterval(&ledTimer,1000);

	setTimerInterval(&motorRampTimer,100);
	setTimerInterval(&checkCANTimer,100);
	setTimerInterval(&watchDog,100);


	//initMotors();


	SetNeoPixRGB(100,0,0);
//  while(1) {
//    //rainBow();
//  }
	//USART1_Flush();
	while(1)
	{

		//toggleLED(6);
//    while(1) {
//      printUART1();
//    }
		//BLINK A LIGHT
		if(timerDone(&ledTimer))
		{
			//#ifdef NEW_MAIN
//      if(flipFlop)
//      {
//        setLED(LED6,ON);
//        flipFlop=false;
//      }
//      else
//      {
//
//        flipFlop=true;
//        setLED(LED6,OFF);
//
//      }

			//PORTB ^= (1<<0);
			//ToSendCAN(0, RouterCardAddress);
			//ToSendCAN(1, getMacroCommand());
			//ToSendCAN(2, getMacroSubCommand());
			//sendDataCAN(NavigationAddress);

//      #else
//        toggleLED(TEST1);
//      #endif
		}
		// Toggle WatchDog Pin
//    if(timerDone(&watchDog))
//    {
//      DDRB ^= (1<<0); //watch dog pin
//    }



		//UPDATE THE COMMUNICATIONS
		//updateComms();
		updateComs2();


#ifdef USE_BUTTONS_TEST
		//buttonsInputTest();
#endif

	}
}