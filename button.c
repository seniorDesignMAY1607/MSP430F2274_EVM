/*
 * button.c
 *
 *  Created on: Apr 8, 2016
 *      Author: nbergman
 */

#include "button.h"

//Navigation buttons -- Port 2
#define PB_UP	BIT0
#define PB_DN	BIT1
#define PB_LF	BIT2
#define PB_RT	BIT3
#define PB_SEL	BIT4

//Power switcg -- Port 3
#define PWR_SW_PIN	BIT3

//#define PWR_ON_LED 	BIT4 //Port 3

//Initialize ports for buttons, and proj_on switch
void button_initPorts(void)
{
	P2DIR &= ~(PB_UP | PB_DN | PB_LF | PB_RT | PB_SEL);
	//P2IE |= (PB_UP | PB_DN | PB_LF | PB_RT | PB_SEL);
	P3DIR &= ~(PWR_SW_PIN);


}


//Check for change in power switch and toggle current state
void button_checkPowerKey(powerState_t *currentState)
{
	//TODO: change to pin change interrupt



	switch(*currentState)
	{
		case OFF:
			if(P3IN & PWR_SW_PIN) *currentState = TURN_ON;
			//Else Nothing
			break;
		case TURN_ON:
			*currentState = ON;
			break;

		case ON:
			if(!(P3IN & PWR_SW_PIN)) *currentState = TURN_OFF;
			//Do Nothing
			break;

		case TURN_OFF:
			*currentState = OFF;
			//P3OUT |= PWR_ON_LED;
			break;

		default:
			//Error undefined state
			P3OUT |= BIT5; //Error led
			break;
	}
}

