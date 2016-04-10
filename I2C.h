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

#include "util.h"
#include "timer.h"

#define DPP_WRITE_ADDR 		0x36
#define DPP_READ_REQ_ADDR 	0x36
#define DPP_READ_ADDR		0x37

//Initialize I2C for 100kHz
//USCI_B0
//SDA = 3.1
//SCL = 3.2
void i2c_init(void);

void i2c_sendData(uint8_t address, uint8_t *data, uint8_t numBytes);

#endif /* I2C_H_ */
