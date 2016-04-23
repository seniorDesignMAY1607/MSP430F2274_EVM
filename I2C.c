/*
 * I2C.c
 *
 *  Created on: Apr 9, 2016
 *      Author: nbergman
 */

#include "I2C.h"

typedef struct {
	uint8_t addr;
	uint8_t	subAddr;
	uint8_t *data;
	uint8_t length;
	uint8_t curPos;
	uint8_t *flags;
}i2c_frame_t;


i2c_frame_t *__i2c_frameBuffer;

uint8_t __i2c_bufferStart, __i2c_bufferEnd;


typedef enum {
	NOT_BUSY,
	IS_BUSY,
	NACK
}i2c_status_t;


i2c_status_t __i2c_currentStatus = NOT_BUSY;

//Transmit  Empty / Receive Full
#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCIAB0TX_ISR(void)
{
	if(__i2c_currentStatus == NOT_BUSY) {
		IFG2 &= ~UCB0TXIFG;
		return;
	}

	i2c_frame_t *curFrame = &__i2c_frameBuffer[__i2c_bufferStart];

	//TX Buffer Empty -- Need to check for Mode?
	if(IFG2 & UCB0TXIFG)
	{
		if(UCB0CTL1 & UCSTTIFG) //Send first data checks for STT flag
		{
			UCB0TXBUF = curFrame->subAddr;
		}else
		{
			P3OUT ^= BIT5;
			UCB0TXBUF = curFrame->data[curFrame->curPos++];

			//Check for rx enough data.... Need to verify
			if(curFrame->curPos >= curFrame->length)
			{
				*(curFrame->flags) |= I2C_SENT; //Sent flag
				*(curFrame->flags) &= ~I2C_IN_BUFFER;
				__i2c_bufferStart = (__i2c_bufferStart + 1) & BUFFER_MASK;//Increment to next data

				//Check for more data in buffer
				if(__i2c_bufferStart == __i2c_bufferEnd)
				{
					__i2c_currentStatus = NOT_BUSY;
					UCB0CTL1 |= UCTXSTP; // Send Nack followed by Stop Condition
				}else
				{
					UCB0I2CSA = curFrame->addr;

					//Set TX / !RX
					if( ( *(curFrame->flags) & I2C_TX_MODE) )
					{
						UCB0CTL1 |= UCTR; //TX Mode
					}else
					{
						UCB0CTL1 &= ~UCTR; 	//RX Mode
					}

					UCB0CTL1 |= UCSTTIFG; //Repeated start -- Retransmit slave address

				}
			}
		}

	}
	else //RX Buffer Full
	{
		P3OUT |= BIT5; //Debug

		//Save data to frame
		curFrame->data[curFrame->curPos++] = UCB0RXBUF;

		//Check for rx enough data.... Need to verify
		if(curFrame->curPos >= curFrame->length)
		{
			*(curFrame->flags) |= I2C_SENT; //Sent flag
			__i2c_bufferStart = (__i2c_bufferStart + 1) & BUFFER_MASK;//Increment to next data

			//Check for more data in buffer
			if(__i2c_bufferStart == __i2c_bufferEnd)
			{
				__i2c_currentStatus = NOT_BUSY;
				UCB0CTL1 |= UCTXSTP; // Send Nack followed by Stop Condition
			}else
			{
				UCB0I2CSA = curFrame->addr;

				//Set TX / !RX
				if( ( *(curFrame->flags) & I2C_TX_MODE) )
				{
					UCB0CTL1 |= UCTR; //TX Mode
				}else
				{
					UCB0CTL1 &= ~UCTR; 	//RX Mode
				}

				UCB0CTL1 |= UCSTTIFG; //Repeated start -- Retransmit slave address

			}
		}
	}
}


//I2C Status ISR -- Nack and St
#pragma vector = USCIAB0RX_VECTOR
__interrupt void USCIAB0RX_ISR(void)
{
	//Check for Start Condition -- Only in Slave Mode
	if(UCB0STAT & UCSTTIFG)
	{
		//Put first data (Sub Address for this application in buffer after slave ACK
		UCB0TXBUF = __i2c_frameBuffer[__i2c_bufferStart].subAddr;
	}
	//Move pointer and start next buffer
	else if(UCB0STAT & UCSTPIFG)
	{
		//This should not be called
		//Only when recieving stop
	}

	//received nack, set data flag, stop bit
	else if(UCB0STAT & UCNACKIFG)
	{
		P3OUT |= BIT4;
		UCB0CTL1 |= UCTXSTP; // Send Nack followed by Stop Condition
		UCB0STAT &= ~UCNACKIFG;
		*(__i2c_frameBuffer[__i2c_bufferStart].flags) |= I2C_NACK;
		__i2c_currentStatus = NACK;
	}
}




//Initialize I2C for 100kHz
//USCI_B0
//SDA = 3.1
//SCL = 3.2
void i2c_masterInit(void)
{
	//Initialize FIFO Buffer
	__i2c_frameBuffer = (i2c_frame_t *) malloc(BUFFER_SIZE * sizeof(__i2c_frameBuffer));
	__i2c_bufferStart = 0;
	__i2c_bufferEnd = 0;


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

	//Interrupt Enable
	UCB0I2CIE |=  UCNACKIE;	//Start Condition and Nack IE
	IE2 |= UCB0TXIE | UCB0RXIE;			//TX ISR -- TX/RX Buffer
										//RX ISR -- I2C Status information
	//TODO: Enable Interupts

}

//Send data over I2C. Busy flag is polled per cycle until data is sent.
//Returns 1 if something went wrong.
uint8_t i2c_sendPolledData(uint8_t address, uint8_t data[], uint8_t numBytes)
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
		UCB0TXBUF = data[i]; //Send data if ACK
		//Busy and wait
		//flag = (uint8_t) IFG2;
		while(!(IFG2 & UCB0TXIFG))
		{
			if(UCB0STAT & UCNACKIFG)
				{
					UCB0CTL1 |= UCTXSTP;
					return I2C_NO_ACK;
				}
				//timer_waitMicro(1);
				//P3OUT ^= BIT4;

		}
	}
	UCB0CTL1 |= UCTXSTP; // Stop Condition

	if(UCB0STAT & UCNACKIFG) return I2C_NO_ACK; //Check for I2C ERROR
	else return I2C_PASS;


}



// Add i2c frame to the FIFO for sending
//Returns length of buffer, -1 if buffer is full
int8_t i2c_addToQueue(uint8_t addr, uint8_t subAddr, uint8_t data[], uint8_t length, uint8_t *flags)
{
	uint8_t bufferStack = BUFFER_LENGTH(__i2c_bufferStart, __i2c_bufferEnd);

	//Double check TX flag is set high
	*flags |= I2C_TX_MODE;

	if(bufferStack == BUFFER_MASK)
	{
		*flags |= I2C_BUFFER_OVF;
		return -1;
	}


	//Push new data into buffer at the end location
	i2c_frame_t *curFrame = &__i2c_frameBuffer[__i2c_bufferEnd];

	//Fill current frame
	curFrame->addr = addr;
	curFrame->subAddr = subAddr;
	curFrame->data = data;
	curFrame->length = length;
	curFrame->flags = flags;
	curFrame->curPos = 0;

	//Increment bufferEnd and loop if necessary
	__i2c_bufferEnd = (__i2c_bufferEnd + 1) & BUFFER_MASK;
	//Set status flag
	*flags |= I2C_IN_BUFFER;



	//Start I2C if necessary
	i2c_startTransacting();


	return bufferStack + 1; //Return buffer length
}


/* Does not need read function -- All R/W info is passed with flag variable and
 * taken care of in interrupt routine

//Add Read command to the queue
//Returns length of buffer, -1 if buffer is full
int8_t i2c_readData(uint8_t addr, uint8_t subAddr, uint8_t data[], uint8_t length, uint8_t *flags);
{
	uint8_t length = BUFFER_SIZE(__i2c_bufferStart, __i2c_bufferEnd);
	//Double check TX flag is set high
	&flag &= ~I2C_TX_MODE;

	if(length == BUFFER_SIZE)
	{
		&flags |= I2C_BUFFER_OVF;
		return -1;
	}
	//Push new data into buffer at the end location
	__i2c_frameBuffer *curFrame = &__i2c_frameBuffer[__i2c_bufferEnd];

	//Fill current frame
	curFrame->addr = addr;
	curFrame->subAddr = subAddr;
	curFrame->data = data;
	curFrame->length = length;
	curFrame->flags = flags;

	//Increment bufferEnd and loop if necessary
	__i2c_bufferEnd = (__i2c_bufferEnd + 1) & BUFFER_MASK;

	//Set status flag
	&flag |= I2C_IN_BUFFER;

	return length + 1; //Return buffer length

}
*/


//Start transacting if not already transacting
void i2c_startTransacting(void)
{
	//Check if status is not busy and i2c module is not in the middle of anything
	//if(!(UCB0STAT & UCBBUSY))
	if(__i2c_currentStatus == NOT_BUSY)
	{
		__i2c_currentStatus = IS_BUSY;

		//Set TX / !RX
		if( ( *(__i2c_frameBuffer[__i2c_bufferStart].flags) & I2C_TX_MODE) )
		{
			UCB0CTL1 |= UCTR; //TX Mode
		}
		else
		{
			UCB0CTL1 &= ~UCTR; 	//RX Mode

		}

		//Set device Address
		UCB0I2CSA = __i2c_frameBuffer[__i2c_bufferStart].addr;

		//Generate start condition
		UCB0CTL1 |= UCTXSTT;
		//Interrupts take over from here
	}
}





void i2c_rxNack(uint8_t* i2c_status)
{
	UCB0STAT &= ~UCNACKIFG;
	*i2c_status = 0;
}


