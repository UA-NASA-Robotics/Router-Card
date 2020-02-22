/*
 * PeripheralSystems.h
 *
 * Created: 11/8/2019 9:23:15 AM
 *  Author: Seth Carpenter
 *
 *
 * System wiki : https://github.com/UA-NASA-Robotics/ROCKEE_Router-Card/wiki
 */
#ifndef PERIPHERALSYSTEM_H
#define PERIPHERALSYSTEM_H
#include <stdbool.h>
#include <stdint.h>
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                       Peripheral System Methods                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////




/// Returns if the registered systems of the robot have replied with a ready status
/// This information is gathered form the Global CAN bus and will be monitored
/// regularly.
bool isSystemReady(uint16_t _mask);

/* parses out the macro data that has been received on the can bus */
uint16_t updateMacroCommand();
/* returns the current macro of the system */
uint16_t getCurrentMacro();

uint16_t getSystemStatus();

void getSystemLocData();

#endif