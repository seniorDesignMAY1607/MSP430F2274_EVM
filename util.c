/*
 * util.c
 *
 *  Created on: Apr 7, 2016
 *      Author: nbergman
 */

#include "util.h"


//Set 1MHz internal clock with calibration
void util_initClock(void)
{
	//Check for valid calibration data for 1MHZ clock
	if((CALDCO_1MHZ != 0xFF) || (CALBC1_1MHZ != 0xFF))
	{
		BCSCTL1 = CALBC1_1MHZ;
		DCOCTL = CALDCO_1MHZ;
	}else
	{
		//Manual Override
		//TODO: Add flag or backup clock
		BCSCTL1 = RSEL2 | RSEL1 | RSEL0;
		DCOCTL = DCO1 | DCO0;
	}
}
