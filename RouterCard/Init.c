/*
 * Init.c
 *
 * Created: 9/6/2017 10:57:22 AM
 *  Author: Zac
 */ 

#include "Init.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "AT90CAN_UART.h"
#include "CommsDefenition.h"
#include "general.h"
#include "LEDs.h"
#include "CANFastTransfer.h"
#include "can.h"
#include "Timer.h"
#include "FastTransfer.h"
#include "commsReceive.h"

static FILE mystdout = FDEV_SETUP_STREAM(USART0_put_C, NULL,
_FDEV_SETUP_WRITE);

//Time keeps for comms management
extern timer_t ledTimer,  checkCANTimer;

void initialize(void)
{
		sei();
		stdout = &mystdout;
		//Startup the USARTs
		USART1_Init(115200); //control box
		USART0_Init(115200); //beacon
		USART0_put_C(0x13);
		printf("Startup \r\n");
		
		//Setup the I/O ports
		PORTA	= 0b01111111;
		PORTF	= 0b11111000;
		DDRA	= 0b11111111; //0-6 are outputs, 7 is an input (jumper for competition mode) //(Currently an output for debug)
		DDRF	= 0b11111000; //3-7 are outputs

		for(int i=1; i <= NUMBER_OF_LEDS; i++)
		{
			setLED(i,ON);
			_delay_ms(50);
		}
		
		_delay_ms(3000);
		
		for(int i=1; i <= NUMBER_OF_LEDS; i++)
		{
			setLED(i,OFF);
			_delay_ms(50);
		}
		
		//Init the CAN here
		can_init();
		
		//Start communciations handlers
		initFastTransfer();
		initCANFastTransfer();
		
		for(int i=1; i <= NUMBER_OF_LEDS; i++)
		{
			setLED(i,ON);
		}
		#ifndef DISABLE_MOTOR_SYSTEMS
		//Init the motors
		initMotors();
		#endif
		
		//Initialize the timer0 and timer systems
		initTimer0();
		setTimerInterval(&ledTimer,1000);
		setTimerInterval(&checkCANTimer,50);
		setupCommsTimers();
		printf("Go \r\n");	
		for(int i=1; i <= NUMBER_OF_LEDS; i++)
		{
			setLED(i,OFF);
		}
	
}