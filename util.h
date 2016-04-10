/*
 * util.h
 *
 *  Created on: Apr 7, 2016
 *      Author: nbergman
 */

#ifndef UTIL_H_
#define UTIL_H_

#include <msp430f2274.h>
#include <stdint.h>

#include "dpp3438.h"


//Set 1MHz internal clock with calibration
void util_initClock(void);


#endif /* UTIL_H_ */
