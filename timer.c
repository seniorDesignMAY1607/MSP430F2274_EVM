/*
 * timer.c
 *
 *  Created on: Apr 7, 2016
 *      Author: nbergman
 */

#include "timer.h"

#define MAX_TIMERA_PERIOD 	65536 //# of microsecond for 1MHz clock  65535 / 1000000000

volatile uint32_t ms_tick_count = 0;	//global count for ms
volatile uint16_t timerA_overflows = 0;		//Count Overflows

//Used for waitMillis -- Increment global millisecond count
#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMERA0_ISR(void)
{
	TA0CCTL0 &= ~CCIE;
	//P3OUT ^= BIT5; //Debug
	ms_tick_count++;
	TA0CCTL0 |= CCIE;
}



/*
#pragma vector = TIMERA1_VECTOR;
__interrupt void TIMERA1_ISR(void)
{
	TA0CCTL0 &= ~CCIE;
	P3OUT ^= BIT4;
	timerA_overflows++; //Not used at the moment//Not used at the moment
	TA0CCTL0 |= CCIE;
}
*/

//Initialize timer
void timer_initA0()
{
	TACTL |= TASSEL_2;
	//TACTL |= TASSEL_2 | MC0;//Always running
	TACCTL0 |= CCIE; //Interupt enable for timer A0
	TACCR0 = 1000;
	__bis_SR_register(GIE); //Global Interrupt Enable
}

//Blocks for a number of milliseconds
void timer_waitMilli(uint16_t theTime)
{
	TACTL |= MC0;//Always running
	ms_tick_count = 0; //Reset millisecond counter

	while(ms_tick_count < theTime )
	{
		//Do Nothing
	}

	TACTL |= TACLR;

}

//Blocks for a number of microseconds
//Starts and resets TimerA
void timer_waitMicro(uint16_t theTime)
{


	//uint16_t finishTime = theTime;
	TACTL |= MC1; //Contiuous Mode

	while(1)
	{
		if(TAR >= theTime || (TAIV & TAIFG)) break;

	}
	TACTL |= TACLR;
}


