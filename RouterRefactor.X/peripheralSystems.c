/*
 * peripheralSystems.c
 *
 * Created: 11/8/2019 9:22:51 AM
 *  Author: Seth Carpenter
 *
 * System wiki : https://github.com/UA-NASA-Robotics/ROCKEE_Router-Card/wiki
 */
#include "PeripheralSystems.h"
//#include "CANFastTransfer.h"

//#include "GlobalCAN_IDs.h"
#include "CommsIDs.h"
#include "commsReceive.h"
#include <stdint.h>

uint16_t currentMacro = 0;
uint16_t previousMacro = 0;

bool isSystemReady(uint16_t _mask)
{
	/* Make sure the mask only is looking for devices that are actually on the bus */
	_mask &= ((0xFFFF >> 10) - GLOBAL_DEVICES);
	/* Getting the status of all the devices that have sent in a status */
	uint16_t status = getSystemStatus() & ((0xFFFF >> 10) - GLOBAL_DEVICES);
	//printf("status: %d\nmask: %d\nResult: %d\n",status,_mask,(status & _mask) == _mask);
	/* Return true if the required statuses are valid */
	return ((status & _mask) == _mask);
}

uint16_t getSystemStatus()
{
	int i;
    //uint16_t i;
	uint16_t isReady = 0x00;
	/* Looping through all the devices on the CAN bus */
	for(i = 1; i<= GLOBAL_DEVICES+1; i++)
	{
		if(getGBL_CANFTFlag(getGBL_DEVICE_STATUS(i)))
		{
			/* if there is any device that has a '0' in the first
			   bit of  their status word isReady will become false*/
			isReady |= (getGBL_CANFTdata(getGBL_DEVICE_STATUS(i)) & 0x01) << (i);

		}
	}
	return isReady;
}
// Looks at all the devise on the Global bus and grabs what Macro they are running
// Returns a 16 bit value with '1's in the position that represents the macro number
uint16_t updateMacroCommand()
{
	int rx_macro = 0x0000;
	//int x;
	// Set the corresponding macro bit to 1 if active for all devices on the GBL bus
	int i;
	for(i = 1; i<= GLOBAL_DEVICES+1; i++)
	{
		if(getGBL_CANFTdata(getGBL_MACRO_INDEX(i))) {
			rx_macro |= getGBL_CANFTdata(getGBL_MACRO_INDEX(i));
		}
	}
	if(rx_macro != previousMacro)
	{
		previousMacro = currentMacro;
		currentMacro = rx_macro;

	}
	return rx_macro;
}
uint16_t getCurrentMacro()
{
	// ? //
	updateMacroCommand();
	return updateMacroCommand();
}
void getSystemLocData()
{
    /*
	int x = getGBL_CANFTdata(getGBL_INDEX(MASTER_CONTROLLER,DATA_0));
	int y = getGBL_CANFTdata(getGBL_INDEX(MASTER_CONTROLLER,DATA_1));
	int h = getGBL_CANFTdata(getGBL_INDEX(MASTER_CONTROLLER,DATA_3));
	*/
     /*
	FT_ToSend(&Control_ft_handle, 10, x);
	FT_ToSend(&Control_ft_handle, 11, y);
	FT_ToSend(&Control_ft_handle, 12, h);
	FT_Send(&Control_ft_handle, CONTROL_BOX);
	*/
}