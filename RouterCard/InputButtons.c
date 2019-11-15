/*
 * InputButtons.c
 *
 * Created: 5/4/2017 9:57:10 AM
 *  Author: Zac
 */
#include "InputButtons.h"
#include "Config.h"
#include "CANFastTransfer.h"
#include "FastTransfer.h"
#include "Definitions.h"
#include  "CommsDefenition.h"
#include "MacroCommands.h"
#include "LEDs.h"
#include "commsReceive.h"
#include <avr/io.h>


bool buttonPressed(buttonList_t b)
{
	switch(b)
	{
#ifndef NEW_MAIN
	case SPECIAL:
		return (!(PINC & (1<<PINC0)));
		break;
	case DWN:
		return (!(PINC & (1<<PINC1)));
		break;
	case UP:
		return (!(PINC & (1<<PINC2)));
		break;
	case BCK:
		return (!(PINA & (1<<PINA7)));
		break;
#else
	case SPECIAL:
		return (!(PINC & (1<<PINC0)));
		break;
	case DWN:
		return (!(PINC & (1<<PINC1)));
		break;
	case UP:
		return (!(PINC & (1<<PINC2)));
		break;
	case BCK:
		return (!(PING & (1<<PING2)));
		break;
#endif
	}
	return false;
}

// #define LEFTMOTOR_TEST
// #define RIGHTMOTOR_TEST
// #define BUCKET_TEST
// #define ARM_TEST

void buttonsInputTest(void)
{
	if(buttonPressed(DWN))
	{
#ifdef LEFTMOTOR_TEST
		LeftMotor_SetVelNoCommsSafety(1500);
#elif defined RIGHTMOTOR_TEST
		//RightMotor_SetVelNoCommsSafety(1500);
		RightMotorSendVel(100);
#elif defined ARM_TEST
		ArmMotor_SetVelNoCommsSafety(100);
#elif defined BUCKET_TEST
		BucketMotor_SetVelNoCommsSafety(-200);
		//long Resolution = 4096;
		//long Ratio = 50;
		//long counts = Resolution*Ratio;
		//BucketMotor_MoveCounts(counts); //4096 counts/rev * 50:1 gear box ratio.
#endif
		_delay_ms(10);
	}
	else if(buttonPressed(UP))
	{
#ifdef LEFTMOTOR_TEST
		LeftMotor_SetVelNoCommsSafety(-1500);
#elif defined RIGHTMOTOR_TEST
		//RightMotor_SetVelNoCommsSafety(-1500);
		RightMotorSendVel(-100);
#elif defined ARM_TEST
		ArmMotor_SetVelNoCommsSafety(-500);
#elif defined BUCKET_TEST
		BucketMotor_SetVelNoCommsSafety(200);
#endif
		_delay_ms(10);
	}
	else if(buttonPressed(BCK))
	{
#ifdef LEFTMOTOR_TEST
		LeftMotor_SetVelNoCommsSafety(0);
#elif defined RIGHTMOTOR_TEST
		RightMotorSendVel(0);
		//RightMotor_SetVelNoCommsSafety(0);
#elif defined ARM_TEST
		ArmMotor_SetVelNoCommsSafety(0);
#elif defined BUCKET_TEST
		BucketMotor_SetVelNoCommsSafety(0);
#endif
		_delay_ms(10);
	}
	else if(buttonPressed(SPECIAL))
	{
		static bool ON_OFF=false;
		if(!ON_OFF)
		{
			setLED(4,ON);
			//Send to the sensor/navi an update of the macro state
			ToSendCAN(0, ROUTER_ADDRESS);  //From Router Card
			ToSendCAN(1, TURNING);        //Macro Command
			ToSendCAN(2, 45);         //Macro sub-command
			sendDataCAN(MASTER_ADDRESS);      //To sensor card
			ON_OFF=true;
			setMacroSubCommand(45);
			setMacroCommand(TURNING);
		}
		else
		{
			setLED(4,OFF);
			//Send to the sensor/navi an update of the macro state
			ToSendCAN(0, ROUTER_ADDRESS);  //From Router Card
			ToSendCAN(1, 0);        //Macro Command
			ToSendCAN(2, 0);          //Macro sub-command
			sendDataCAN(MASTER_ADDRESS);      //To sensor card
			ON_OFF=false;
			setMacroSubCommand(0);
			setMacroCommand(0);

		}
		_delay_ms(250);

	}
}

