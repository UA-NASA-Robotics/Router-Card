/*! \file
  generic CAN bus driver for AT90CAN128

  privides interrupt-driven reception and polled transmission of can packets

  \author Dr. Klaus Schaefer \n
  Hochschule Darmstadt * University of Applied Sciences \n
  schaefer@eit.h-da.de \n
  http://kschaefer.eit.h-da.de

  You can redistribute it and/or modify it
  under the terms of the GNU General Public License.\n
  It is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; \n
  without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n
  See http://www.gnu.org/copyleft/gpl.html for mor details.

*/

//#define BAUD_1000K
#define BAUD_500K
//#define BAUD_125K

#include "avr/interrupt.h"
#include "assert.h"
#include "can.h"
#include "Config.h"
#include "AT90CAN_UART.h"
#define F_CPU 16000000
#include <util/delay.h>
#define Test1 3
CAN_cbf canlist[NO_MOBS]; //!< can callback list for RX channels, indexed by MOB number

//! CAN interrupt service routine

//! presently only used for reception of CAN packets


//THIS LOOKS OKAY
ISR( CANIT_vect)
{
	unsigned i;

	//Process packet (static??)
	static CAN_packet packet;

	//CANPage saving
	char save_canpage=CANPAGE;

	unsigned mob=CANHPMOB; // get highest prio mob
	CANPAGE = mob & 0xf0;

	mob >>= 4; // -> mob number 0..15
	//----------------RECEIVING DATA (RX)----------------
	if((CANSTMOB & ~0x20) ==0)
	{
		register char length;
		packet.id=(CANIDT2>>5) | (CANIDT1 <<3);

		//Identify the length of the packet
		length=CANCDMOB & 0x0f;
		packet.length=length;

		//Push the packet in to the storage
		for (i = 0; i <length; ++i)
		{
			packet.data[i] = CANMSG;
		}
		CANSTMOB=0;   // reset INT reason
		CANCDMOB=0x80;  // re-enable RX on this channel
		canlist[ mob]( &packet, mob);
		CANPAGE=save_canpage; // restore CANPAGE
	}
	//----------------SENDING DATA (TX) ----------------
	else if((CANSTMOB & ~0x40) ==0)
	{
		canlist[ mob]( &packet, mob); //if data remains in send buffer

		if(packet.length > 0)
		{
			//Set the TX ID
			CANIDT1 = packet.id >>3;
			CANIDT2 = packet.id <<5;
			CANIDT3 = 0;
			CANIDT4 = 0;

			//PUSH THE PACKET
			if(packet.length == 9)
			{
				for (unsigned char cnt=0; cnt < 8; ++cnt)
					CANMSG = packet.data[cnt];
			}
			else
			{
				for (unsigned char cnt=0; cnt < packet.length; ++cnt)
					CANMSG = packet.data[cnt];
			}
			CANSTMOB = 0;   // reset INT reason
			CANCDMOB = 0x40 + packet.length;
		}
		else //Disable mob to stop interrupts (NO MORE TX TO SEND)
		{
			CANSTMOB=0;   // reset INT reason
			CANCDMOB = 0x00;
		}
		CANPAGE=save_canpage; // restore CANPAGE

	}
	else if((CANSTMOB & ~0x80) ==0) //DLC warning
	{

		register char length;
		packet.id=(CANIDT2>>5) | (CANIDT1 <<3);
		length=CANCDMOB & 0x0f;
		packet.length=length;
		for (i = 0; i <length; ++i)
		{
			packet.data[i] = CANMSG;
		}
		CANSTMOB=0;   // reset INT reason
		CANCDMOB=0x80;  // re-enable RX on this channel
		canlist[ mob]( &packet, mob);
		CANPAGE=save_canpage; // restore CANPAGE
	}
	else if((CANSTMOB & ~0xA0) ==0) //DLC warning and RX ok
	{

		register char length;
		packet.id=(CANIDT2>>5) | (CANIDT1 <<3);
		length=CANCDMOB & 0x0f;
		packet.length=length;
		for (i = 0; i <length; ++i)
		{
			packet.data[i] = CANMSG;
		}
		CANSTMOB=0;   // reset INT reason
		CANCDMOB=0x80;  // re-enable RX on this channel
		canlist[ mob]( &packet, mob);
		CANPAGE=save_canpage; // restore CANPAGE
	}
	else if((CANSTMOB & ~0x10) ==0) //Bit Error warning
	{

		//handle transmission error
	}

}

bool can_tx_kick(char mob) //used to start transmission, interrupt will send rest of buffer
{
	static CAN_packet packet;
//  if (
//    (  CANCDMOB & 0b11000000) // if MOB in use
//    &&
//    ( (CANSTMOB & 0x40) ==0)   // TX not ready
//  ) {
//    USART0_put_C(0xBB);
//    return false;
//  }
	canlist[ mob]( &packet, mob); //if data remains in send buffer
	unsigned char cnt;
	CANPAGE = mob << 4;
	if(packet.length == 0)
	{
		return false; //no data to send
	}

	CANSTMOB = 0x00;      // cancel pending operation
	CANCDMOB = 0x00;
	if( packet.length!=RTR)
	{
		CANIDT1=packet.id >>3;
		CANIDT2=packet.id <<5;
		CANIDT3 = 0;
		CANIDT4 = 0;
		if(packet.length == 9)
		{
			for (cnt=0; cnt < 8; ++cnt)
				CANMSG = packet.data[cnt];
		}
		else
		{
			for (cnt=0; cnt < packet.length; ++cnt)
				CANMSG = packet.data[cnt];
		}
	}

	else
	{
		CANIDT1=packet.id >>3;
		CANIDT2=packet.id <<5;
		CANIDT3 = 0;
		CANIDT4 = 1<<RTRTAG;
	}
	CANCDMOB = 0x40 + packet.length;

	return true;
}

bool can_tx( char mob,  CAN_packet *packet) //leaving this here for legacy
{
	unsigned cnt;
	ASSERT( packet->id <= 0x7ff);
	ASSERT( packet->length <=8);
	ASSERT( mob <=14);
	CANPAGE = mob << 4;

	if (
		(  CANCDMOB & 0b11000000) // if MOB in use
		&&
		( (CANSTMOB & 0x40) ==0)   // TX not ready
	)
		return false;

	CANSTMOB = 0x00;      // cancel pending operation
	CANCDMOB = 0x00;
	if( packet->length!=RTR)
	{
		CANIDT1 = packet->id >>3;
		CANIDT2 = packet->id <<5;
		CANIDT3 = 0;
		CANIDT4 = 0;
		for (cnt=0; cnt < packet->length; ++cnt)
			CANMSG = packet->data[cnt];
	}
	else
	{
		CANIDT1 = packet->id >>3;
		CANIDT2 = packet->id <<5;
		CANIDT3 = 0;
		CANIDT4 = 1<<RTRTAG;
	}
	CANCDMOB = 0x40 + packet->length;

	return true;
}

bool prepare_rx( char mob, unsigned id, unsigned idmask, CAN_cbf callback)
{
	if( mob >= 15)        // illegal MOB number
		return true;
	canlist[ (unsigned)mob]=callback;

	CANPAGE = mob << 4;
	if( callback==0)      // release this MOB
	{
		CANSTMOB = 0x00;      // cancel pending operation
		CANCDMOB = 0x00;
		unsigned mask=1<<mob;
		CANIE2 &=  ~mask;
		CANIE1 &= ~(mask>>8);
		return false;
	}
	if( CANCDMOB & 0b11000000)  // if MOB already in use
		return true;      // no vacancy ...
	CANSTMOB = 0x00;      // cancel pending operation
	CANCDMOB = 0x00;
	CANHPMOB = 0x00;    // enable direct mob indexing, see docu
	CANIDT1=id >>3;
	CANIDT2=id <<5;
	CANIDM1=idmask >>3;
	CANIDM2=idmask <<5;
	CANIDM3=0;
	CANIDM4=0;
	CANCDMOB=0x80; // enable RX
	unsigned mask=1<<mob;
	CANIE2 |=  mask;
	CANIE1 |= (mask>>8);
	return false;
}

bool prepare_tx( char mob, unsigned id, unsigned idmask, CAN_cbf callback)
{
	if( mob >= 15)        // illegal MOB number
		return true;
	canlist[ (unsigned)mob]=callback;

	CANPAGE = mob << 4;
	if( callback==0)      // release this MOB
	{
		CANSTMOB = 0x00;      // cancel pending operation
		CANCDMOB = 0x00;
		unsigned mask=1<<mob;
		CANIE2 &=  ~mask;
		CANIE1 &= ~(mask>>8);
		return false;
	}
	if( CANCDMOB & 0b11000000)  // if MOB already in use
		return false;     // no vacancy ...
	CANSTMOB = 0x00;      // cancel pending operation
	CANCDMOB = 0x00;
	CANHPMOB = 0x00;    // enable direct mob indexing, see docu
	//CANIDT1=id >>3;
	//CANIDT2=id <<5;
	//CANCDMOB=0x40; // enable TX
	unsigned mask=1<<mob;
	CANIE2 |=  mask;
	CANIE1 |= (mask>>8);
	return true;
}

void can_init( void)
{
	unsigned mob;
#ifdef BAUD_500K
	CANBT1=0x02;  // 16 MHz, 500kbit/s
	CANBT2=0x0C;
	CANBT3=0x37;
	//#elif defined BAUD_125K
	//CANBT1=0x0E;  // 16 MHz, 125kbit/s
	//CANBT2=0x0C;
	//CANBT3=0x37;
#else //defined BAUD_1000K
	CANBT1=0x00;  // 16 MHz, 1000kbit/s
	CANBT2=0x0C;
	CANBT3=0x36;
#endif

	for (mob = 0; mob < NO_MOBS; mob++)
	{
		CANPAGE  = (mob << 4);
		CANSTMOB = 0;
		CANCDMOB = 0;
	}
	CANGCON |= 0x02;
	CANGIE=(1<<ENIT) | (1<< ENRX) | (1<< ENTX);
}

