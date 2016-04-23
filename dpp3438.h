/*
 * dpp3438.h
 *
 *  Created on: Apr 8, 2016
 *      Author: nbergman
 */

#ifndef DPP3438_H_
#define DPP3438_H_

#include <msp430f2274.h>
#include <stdint.h>

#include "timer.h"
#include "util.h"
#include "I2C.h"

#define DPP3438_ADDR	0x1B


//Initialize I2C, Ports, and Other peripherals for DPP3438
void dpp_init(void);

//TUrn on DPP3438, 1.8V power, I2C
void dpp_turnOn(void);

//Turn off DPP3438, 1.8V power, and I2C
void dpp_turnOff(void);



//Change splash screen
void dpp_changeSplashScreen(uint8_t imageNum);

/**
 * Selec image source
 * @param sourceNum number of the source
 *
 */
void dpp_sourceSelect(uint8_t sourceNum);


//Send write command to DPP3438
//Subaddress, Pointer to data and length of data array
void dpp_writePolledCommand(uint8_t subaddress, uint8_t *data, uint8_t numBytes);


//Display curtain
void dpp_dispCurtain(uint8_t color, uint8_t enable);


//Execute splash screen
void dpp_splashScreenExecute(void);

#endif /* DPP3438_H_ */
