/*
 * button.h
 *
 *  Created on: Apr 8, 2016
 *      Author: nbergman
 */

#ifndef BUTTON_H_
#define BUTTON_H_


#include <msp430f2274.h>
#include <stdint.h>
#include "dpp3438.h"


typedef enum{
	OFF,
	TURN_ON,
	ON,
	TURN_OFF
}powerState_t;


//Initialize ports for buttons, and proj_on switch
void button_initPorts(void);

//Check power switch, change state if necessary
void button_checkPowerKey(powerState_t *currentState);




#endif /* BUTTON_H_ */
