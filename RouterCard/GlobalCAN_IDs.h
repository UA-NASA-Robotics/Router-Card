/*
 * GlobalCAN_IDs.h
 *
 * Created: 11/13/2019 9:44:03 PM
 *  Author: John
 */


#ifndef GLOBALCAN_IDS_H_
#define GLOBALCAN_IDS_H_
#include <stdbool.h>
#include "Definitions.h"

#define GLOBAL_DEVICES 10
#define GLOBAL_DATA_IDX_PER_DEV 7



//Complete index of all the
typedef enum {
	DEVICE_STATUS=0,
	DEVICE_MACRO,
	DATA_0,
	DATA_1,
	DATA_2,
	DATA_3,
	DATA_4
} GlobalDeviceData_t;
#define getGBL_DEVICE_STATUS(c) c*GLOBAL_DATA_IDX_PER_DEV + DEVICE_STATUS
#define getGBL_MACRO_INDEX(c) c*GLOBAL_DATA_IDX_PER_DEV + DEVICE_MACRO
// typedef struct
// {
//  // address: This will be used as the offset value for indexing the below arrays
//  int address;
//  // data: Will hold the data received on the Global bus for the specified device
//  int data[GLOBAL_DATA_IDX_PER_DEV];
//  // statusFlag: Will hold the status of the new data having been received
//  bool statusFlag[GLOBAL_DATA_IDX_PER_DEV];
// } GlobalDeviceData_t;

#endif /* GLOBALCAN_IDS_H_ */