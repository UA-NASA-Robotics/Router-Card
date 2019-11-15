/*
 * RouterCard.c
 *
 * Created: 4/13/2016 11:28:41 PM
 *Author: Seth Carpenter
 *
 *
 * System wiki : https://github.com/UA-NASA-Robotics/ROCKEE_Router-Card/wiki
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
#include "assert.h"
#include "CANFastTransfer.h"
#include "commsReceive.h"
#include "LEDs.h"
#include "Config.h"
#include "InputButtons.h"
#include "Timer.h"
#include "Init.h"

#define __AVR_CAN90CAN128



//Time keeps for comms management
timer_t  checkCANTimer;

bool flipFlop= true;

int main(void)
{

	initialize();

	initCOMs();



	setTimerInterval(&checkCANTimer,100);

	SetNeoPixRGB(100,0,0);

	while(1)
	{

		//updateComs2();
		CommunicationsHandle();


#ifdef USE_BUTTONS_TEST
		//buttonsInputTest();
#endif

	}
}