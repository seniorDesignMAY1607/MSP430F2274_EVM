/*
 * I2C.c
 *
 *  Created on: Apr 9, 2016
 *      Author: nbergman
 */

#include "I2C.h"



//Initialize I2C for 100kHz
//USCI_B0
//SDA = 3.1
//SCL = 3.2
void i2c_masterInit(void)
{


	//Master, I2C, Synchronous
	UCB0CTL1 |= UCSWRST;



	UCB0CTL0 |= UCMST | UCMODE1 | UCMODE0 | UCSYNC;

	//SMCLK, Transmitter,
	UCB0CTL1 |= UCSSEL1 | UCTR;
	UCB0BR0 = 10; //Divide by 10 prescalar -- 1MHZ SMCLK

	//config ports
	P3SEL |= BIT1 | BIT2; //See device specific datasheet for selecting alternate pin functions

	//Undo Reset
	UCB0CTL1 &= ~UCSWRST;
	//TODO: Enable Interupts
	//UCB0CTL1 |= UCTXSTP; // Stop Condition

}

//Send data over I2C. Busy flag is polled per cycle until data is sent.
//Returns 1 if something went wrong.
uint8_t i2c_sendPolledData(uint8_t address, uint8_t *data, uint8_t numBytes)
{
	int i = 0;
	int busyCount = 0;

	while(1)
	{
		if(!(UCB0STAT & UCBBUSY)) break;
		else if(busyCount > 20)
		{
			return I2C_BUSY;
		}
		timer_waitMicro(20);
		busyCount++;


	}

	//Set device Address
	UCB0I2CSA = address;

	//Generate start condition
	UCB0CTL1 |= UCTXSTT;


	for(i = 0; i < numBytes; i++)
	{
		//Wait for start flag to go high
		//Put data in TXBUF
		//Busy and wait
		while(!(IFG2 | UCB0TXIFG));

		if(UCB0STAT & UCNACKIFG)
			{
			return I2C_NO_ACK;

			}
		else UCB0TXBUF = *(data); //Send data if ACK

	}

	UCB0CTL1 |= UCTXSTP; // Stop Condition

	if(UCB0STAT & UCNACKIFG) return I2C_NO_ACK; //Check for I2C ERROR
	else return I2C_PASS;

}


void i2c_rxNack(uint8_t* i2c_status)
{
	UCB0STAT &= ~UCNACKIFG;
	*i2c_status = 0;
}

