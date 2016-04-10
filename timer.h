/*
 * timer.h
 *
 *  Created on: Apr 7, 2016
 *      Author: nbergman
 */

#ifndef TIMER_H_
#define TIMER_H_


#include <msp430f2274.h>
#include <stdint.h>

//Initialize timer for main System Tick
void timer_initA0();

//Blocks for a number of milliseconds
void timer_waitMilli(uint16_t theTime);

//Blocks for a number of microseconds
void timer_waitMicro(uint16_t theTime);


#endif /* TIMER_H_ */
