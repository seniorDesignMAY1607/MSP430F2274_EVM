/*
 * dpp3438.c
 *
 *  Created on: Apr 8, 2016
 *      Author: nbergman
 */

#include "dpp3438.h"

#define PROJ_ON_PIN		BIT6	//Port 2
#define HOST_IRQ_PIN 	BIT5	//Port 2


//Initialize I2C, Ports, and Other peripherals for DPP3438
void dpp_init(void)
{
	//Wait for HOST_IRQ to go low
	P2DIR &= ~HOST_IRQ_PIN; //HOST IRQ
	P2DIR |= PROJ_ON_PIN;
	P2OUT &= ~PROJ_ON_PIN; //Proj Off

	while(P2IN & HOST_IRQ_PIN)
	{
		//Do Nothing
		//TODO: Check for timeout
	}

	//Initialize I2C

}

//TUrn on DPP3438, 1.8V power, I2C
void dpp_turnOn(void)
{
	//TODO: Turn on 1.8V power, check status

	P2OUT |= PROJ_ON_PIN;
	while(P2IN & HOST_IRQ_PIN)
	{
		//Do Nothing
		//TODO: Check for timeout
	}

	//TODO: Init I2C
}

//Turn off DPP3438, 1.8V power, and I2C
void dpp_turnOff(void)
{
	//TODO: Stop I2C
	P2OUT &= ~PROJ_ON_PIN;

	timer_waitMilli(500);
	//TODO:Turn off 1.8V Power

}






