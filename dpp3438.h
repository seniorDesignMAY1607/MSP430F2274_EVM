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

//Initialize I2C, Ports, and Other peripherals for DPP3438
void dpp_init(void);

//TUrn on DPP3438, 1.8V power, I2C
void dpp_turnOn(void);

//Turn off DPP3438, 1.8V power, and I2C
void dpp_turnOff(void);

#endif /* DPP3438_H_ */
