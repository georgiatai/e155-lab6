// DS1722.h
// Georgia Tai
// ytai@g.hmc.edu
// Nov. 1, 2025
// Header file for DS1722 temperature sensor

#ifndef DS1722_H
#define DS1722_H

#include "STM32L432KC.h"

#define DS1722_CE_PIN PB1

///////////////////////////////////////////////////////////////////////////////
// Function prototypes
///////////////////////////////////////////////////////////////////////////////

/* Initialize the DS1722 temperature sensor to a specific resolution 
 *    -- resolution: hex byte for resolution in the config register */ 
void initTempSensor(char resolution);

/* Read MSB & LSB, then calculate the temperature */
double readTemp();

#endif