/*
 * I2C.h
 *
 *  Created on: Apr 9, 2016
 *      Author: nbergman
 */

#ifndef I2C_H_
#define I2C_H_

#include <msp430f2274.h>
#include <stdint.h>
#include <stdlib.h>
#include "util.h"
#include "timer.h"



#define BUFFER_SIZE 			16
#define BUFFER_MASK				(BUFFER_SIZE - 1)
#define	BUFFER_LENGTH(str, end)	((end) - (str) & BUFFER_MASK)


//Flag defines
#define I2C_UNSENT 		0x00
#define I2C_IN_BUFFER	BIT0
#define I2C_SENT		BIT1
#define I2C_BUFFER_OVF	BIT2
#define I2C_NACK		BIT3
#define I2C_TX_MODE		BIT4

//Used for polledSend
//TODO: Change out for flag variables
#define I2C_BUSY 1
#define I2C_NO_ACK	2
#define I2C_PASS	3


//Initialize I2C for 100kHz
//USCI_B0
//SDA = 3.1
//SCL = 3.2
void i2c_masterInit(void);

//Send data over I2C. Busy flag is polled per cycle until data is sent.
//Returns 1 if something went wrong.
uint8_t i2c_sendPolledData(uint8_t address, uint8_t data[], uint8_t numBytes);


//Write I2C data of variable length with subaddress
//Returns length of buffer, -1 if buffer is full
int8_t i2c_addToQueue(uint8_t addr, uint8_t subAddr, uint8_t data[], uint8_t length, uint8_t *flags);

//Add Read command to the queue
//Returns length of buffer, -1 if buffer is full
///int8_t i2c_readData(uint8_t addr, uint8_t subAddr, uint8_t data[], uint8_t length, uint8_t *flags);



//Clear nack flag and reset i2c
//TODO: Change to interrupt!!
void i2c_rxNack(uint8_t* i2c_status);

#endif /* I2C_H_ */
