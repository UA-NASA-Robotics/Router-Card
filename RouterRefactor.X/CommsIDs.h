/* 
 * File:   commsReceive.h
 * Author: John, Alex
 *
 * Created on February 14, 2020, 5:31 PM
 */

#ifndef COMMSIDS_H
#define	COMMSIDS_H

#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif
    typedef enum
    {
        CONTROL_BOX = 1,
        POZYX,
        JUICE_BOARD,
        ROUTER_CARD,
        MASTER_CONTROLLER,
        MOTOR_CONTROLLER,
        GYRO_CONTROLLER,
        STRAIN_SENSOR,
        OPTICAL_FLOW,
        RASP_PI,
        LED_CARD,
        LIDAR,
        GLOBAL_ADDRESS = 31
    } Addresses_t;
    typedef enum
    {
        DEVICE_STATUS,
        DEVICE_MACRO,
        DATA_0,
        DATA_1,
        DATA_2,
        DATA_3,
        DATA_4
    } GlobalDeviceData_t;
    #define GLOBAL_DEVICES 10
    #define GLOBAL_DATA_INDEX_PER_DEVICE 7
    #define getGBL_START_INDEX(c) c*GLOBAL_DATA_INDEX_PER_DEVICE
    #define getGBL_DEVICE_STATUS(c) c*GLOBAL_DATA_INDEX_PER_DEVICE + DEVICE_STATUS
    #define getGBL_MACRO_INDEX(c) c*GLOBAL_DATA_INDEX_PER_DEVICE + DEVICE_MACRO
    #define getGBL_INDEX(c,d)  c*GLOBAL_DATA_INDEX_PER_DEVICE + d
#ifdef	__cplusplus
}
#endif

#endif	/* COMMSIDS_H */