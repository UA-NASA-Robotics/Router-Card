/*
 * LEDs.c
 *
 * Created: 5/3/2017 11:13:47 PM
 *  Author: Zac
 */

#include "LEDs.h"
#include "Timer.h"
#include <util/delay.h>
#include <avr/io.h>


#define NEOPIXALcount_upright 8
#define NEOPIXALcount_arm 25
//
// cRGB_t ledStrand_leftArm[NEOPIXALcount_arm];
// cRGB_t ledStrand_rightArm[NEOPIXALcount_arm];
// cRGB_t ledStrand_leftUpright[NEOPIXALcount_upright];
cRGB_t ledStrand_rightUpright[NEOPIXALcount_upright];
cRGB_t rgbColor[1];

timer_t LED_Flash;
bool ToggleVal = false;
void rainBow()
{
	
	
	
	

	cRGB_t rgbColour;
// Start off with red.
	rgbColour.r = 255;
	rgbColour.g = 0;
	rgbColour.b = 0;

// Choose the colours to increment and decrement.
	for (int decColour = 0; decColour < 3; decColour += 1) {
		int incColour = decColour == 2 ? 0 : decColour + 1;

		// cross-fade the two colours.
		for(int i = 0; i < 255; i += 1) {
			switch (decColour)
			{
			case 0:
				rgbColour.r -= 1;
				break;
			case 1:
				rgbColour.g -= 1;
				break;
			case 2:
				rgbColour.b -= 1;
				break;
			}
			switch (incColour)
			{
			case 0:
				rgbColour.r += 1;
				break;
			case 1:
				rgbColour.g += 1;
				break;
			case 2:
				rgbColour.b += 1;
				break;
			}


			SetNeoPixRGB(rgbColour.r, rgbColour.g, rgbColour.b);
			_delay_us(100);
		}
	}
}
void initLED()
{
	setTimerInterval(&LED_Flash, 1000);
	SetNeoPixRGB(0, 0, 0);
}
cRGB_t *color;
void SetNeoPixRGB(unsigned int R, unsigned G, unsigned int B)
{
	rgbColor[0].r=R;
	rgbColor[0].g=G;
	rgbColor[0].b=B;

	for(int counter = 0; counter <= NEOPIXALcount_upright ; counter++)
	{

		ws2812_setleds(rgbColor,counter,ws2812_pin3);                        // wait for 500ms.

		ws2812_setleds(rgbColor,counter,ws2812_pin4);                        // wait for 500ms.

		//_delay_ms(100);
	}
	for(int counter = 1; counter < NEOPIXALcount_arm; counter++)
	{

		ws2812_setleds(rgbColor,counter,ws2812_pin1);                        // wait for 500ms.
		ws2812_setleds(rgbColor,counter,ws2812_pin2);
		//_delay_ms(100);
	}
}

void flashLedColors(cRGB_t cRGB_1,cRGB_t cRGB_2)
{
	if(timerDone(&LED_Flash)) {
		if(ToggleVal) {
			SetNeoPixRGB(cRGB_1.r, cRGB_1.g, cRGB_1.b);
			ToggleVal = false;
		} else {
			SetNeoPixRGB(cRGB_2.r, cRGB_2.g, cRGB_2.b);
			ToggleVal = true;
		}
	}

}
void setLED(LEDs_ENUMED_t ledNumber, bool state)
{
	int ledToSwitch = 0;
	//HIGH SETS
	if(!state)
	{
		switch(ledNumber)
		{
#ifdef NEW_MAIN
		//LEDs on PORTF
		case LED1:
			ledToSwitch=LED1_P_HIGH;
			break;
		case LED2:
			ledToSwitch=LED2_P_HIGH;
			break;
		case LED7:
			ledToSwitch=LED7_P_HIGH;
			break;
		case LED8:
			ledToSwitch=LED8_P_HIGH;
			break;
		case LED9:
			ledToSwitch=LED9_P_HIGH;
			break;

		//LEDs on PORTA
		case LED3:
			ledToSwitch=LED3_P_HIGH;
			break;
		case LED4:
			ledToSwitch=LED4_P_HIGH;
			break;
		case LED5:
			ledToSwitch=LED5_P_HIGH;
			break;
		case LED6:
			ledToSwitch=LED6_P_HIGH;
			break;
		case LED10:
			ledToSwitch=LED10_P_HIGH;
			break;
		case LED11:
			ledToSwitch=LED11_P_HIGH;
			break;
		case LED12:
			ledToSwitch=LED12_P_HIGH;
			break;
#else
		case TEST1:
			ledToSwitch=TEST1_P_HIGH;
			break;
		case TEST2:
			ledToSwitch=TEST2_P_HIGH;
			break;
		case ERROR1:
			ledToSwitch=ERROR1_P_HIGH;
			break;
		case ERROR2:
			ledToSwitch=ERROR2_P_HIGH;
			break;
#endif
		}
	}
	else
	{
		switch(ledNumber)
		{
#ifdef NEW_MAIN
		//LEDs on PORTF
		case LED1:
			ledToSwitch=LED1_P_LOW;
			break;
		case LED2:
			ledToSwitch=LED2_P_LOW;
			break;
		case LED7:
			ledToSwitch=LED7_P_LOW;
			break;
		case LED8:
			ledToSwitch=LED8_P_LOW;
			break;
		case LED9:
			ledToSwitch=LED9_P_LOW;
			break;

		//LEDs on PORTA
		case LED3:
			ledToSwitch=LED3_P_LOW;
			break;
		case LED4:
			ledToSwitch=LED4_P_LOW;
			break;
		case LED5:
			ledToSwitch=LED5_P_LOW;
			break;
		case LED6:
			ledToSwitch=LED6_P_LOW;
			break;
		case LED10:
			ledToSwitch=LED10_P_LOW;
			break;
		case LED11:
			ledToSwitch=LED11_P_LOW;
			break;
		case LED12:
			ledToSwitch=LED12_P_LOW;
			break;
#else
		case TEST1:
			ledToSwitch=TEST1_P_LOW;
			break;
		case TEST2:
			ledToSwitch=TEST2_P_LOW;
			break;
		case ERROR1:
			ledToSwitch=ERROR1_P_LOW;
			break;
		case ERROR2:
			ledToSwitch=ERROR2_P_LOW;
			break;
#endif
		}


	}
#ifdef NEW_MAIN
	unsigned int portRead;
	switch(ledNumber)
	{
	case LED1:
	case LED2:
	case LED10:
	case LED11:
	case LED12:

		portRead= PINF;
		//if you want the LED off
		if(!state)
		{
			//Turn it high (OFF)
			PORTF = portRead | (ledToSwitch);
		}
		else
		{
			//Turn it low (ON)
			PORTF = portRead & (ledToSwitch);
		}
		break;
	case LED3:
	case LED4:
	case LED5:
	case LED6:
	case LED7:
	case LED8:
	case LED9:
		portRead= PINA;
		//If you want the LED off
		if(!state)
		{
			//Turn the port high (OFF)
			PORTA = portRead | (ledToSwitch);
		}
		else
		{
			//Turn the port low (ON)
			PORTA = portRead & (ledToSwitch);
		}
		break;
	}
#else
	if(!state)
	{
		PORTA= PINA | (ledToSwitch);
	}
	else
	{
		PORTA= PINA & (ledToSwitch);
	}
#endif

}

void toggleLED(LEDs_ENUMED_t ledNumber)
{
	int ledToSwitch=0;
	switch(ledNumber)
	{
#ifdef NEW_MAIN
	//LEDs on PORTF
	case LED1:
		ledToSwitch=LED1_P_HIGH;
		break;
	case LED2:
		ledToSwitch=LED2_P_HIGH;
		break;
	case LED7:
		ledToSwitch=LED7_P_HIGH;
		break;
	case LED8:
		ledToSwitch=LED8_P_HIGH;
		break;
	case LED9:
		ledToSwitch=LED9_P_HIGH;
		break;

	//LEDs on PORTA
	case LED3:
		ledToSwitch=LED3_P_HIGH;
		break;
	case LED4:
		ledToSwitch=LED4_P_HIGH;
		break;
	case LED5:
		ledToSwitch=LED5_P_HIGH;
		break;
	case LED6:
		ledToSwitch=LED6_P_HIGH;
		break;
	case LED10:
		ledToSwitch=LED10_P_HIGH;
		break;
	case LED11:
		ledToSwitch=LED11_P_HIGH;
		break;
	case LED12:
		ledToSwitch=LED12_P_HIGH;
		break;
#else
	case TEST1:
		ledToSwitch=TEST1_P_HIGH;
		break;
	case TEST2:
		ledToSwitch=TEST2_P_HIGH;
		break;
	case ERROR1:
		ledToSwitch=ERROR1_P_HIGH;
		break;
	case ERROR2:
		ledToSwitch=ERROR2_P_HIGH;
		break;
#endif
	}
#ifdef NEW_MAIN
	switch(ledNumber)
	{
	case LED1:
	case LED2:
	case LED10:
	case LED11:
	case LED12:
		//Toggle pin State
		PINF = (ledToSwitch);
		break;
	case LED3:
	case LED4:
	case LED5:
	case LED6:
	case LED7:
	case LED8:
	case LED9:
		//Toggle pin State
		PINA= (ledToSwitch);
		break;
	}
#else
	PINA = ledToSwitch;
#endif
}