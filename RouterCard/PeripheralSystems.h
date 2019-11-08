/*
 * PeripheralSystems.h
 *
 * Created: 11/8/2019 9:23:15 AM
 *  Author: John
 */
#ifndef PERIPHERALSYSTEM_H
#define PERIPHERALSYSTEM_H
#include <stdbool.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                       Peripheral System Methods                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////




/// Returns if the registered systems of the robot have replied with a ready status
/// This information is gathered form the Global CAN bus and will be monitored
/// regularly.
bool isSystemReady();





#endif