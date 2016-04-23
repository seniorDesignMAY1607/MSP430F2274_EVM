/*
 * dpp3438.c
 *
 *  Created on: Apr 8, 2016
 *      Author: nbergman
 */

#include "dpp3438.h"

#define PROJ_ON_PIN		BIT6	//Port 2
#define HOST_IRQ_PIN 	BIT5	//Port 2

#define DPP_ADDR	0x1B

//Defined subaddresses for DPP3438
#define W_SYSTEM_RESET				0x00
#define W_INPUT_SOURCE_SELECT		0x05
#define R_INPUT_SOURCE_SELECT		0X06
#define W_EXT_VID_SOURCE_FORMAT		0X07
#define W_TEST_PATTERN_SELECT		0X0B
#define R_TEST_PATTERN_SELECT		0X0C
#define W_SPLASH_SCREEN_SELECT		0x0D
#define R_SPLASH_SCREEN_SELECT		0X0E
#define R_SPLASH_SCREEN_HEADER		0X0F
#define	W_IMAGE_CROP				0X10
#define R_IMAGE_CROP				0X11
#define W_DISPLAY_SIZE				0X12
#define R_DISPLAY_SIZE				0X13
#define W_DISPLAY_ORIENTATION		0X14
#define R_DISPLAY_ORIENTATION		0X15
#define W_DISPLAY_IMAGE_CURTAIN		0X16
#define R_DISPLAY_IMAGE_CURTAIN		0X17
#define W_IMAGE_FREEZE				0X1A
#define R_IMAGE_FREEZE				0X1B
#define W_LOOK_SELECT				0X22
#define R_LOOK_SELECT				0X23
#define R_SEQUENCE_HEADER_ATTR		0X26
#define R_DMD_SEQUENCER_SYNC_MODE	0X2C
#define W_EXEC_BATCH_FILE			0X2D
#define W_EXTERNAL_INPUT_IMG_SIZE	0X2E
#define R_EXTERNAL_INPUT_IMG_SIZE	0X2F
#define W_SPLASH_SCREEN_EXECUTE		0X35
//BRIGHTNESS CONTROL
#define W_LED_OUT_CONTROL_METHOD	0X50
#define R_LED_OUT_CONTROL_METHOD	0X51
#define W_RGB_LED_ENABLE			0X52
#define R_RGB_LED_ENABLE			0X53
#define W_RGB_LED_CURRENT			0X54
#define R_RGB_LED_CURRENT			0X55
#define R_CAIC_LED_MAX_POWER		0X57
#define W_RGB_LED_MAX_CURRENT		0X5C
#define R_RGB_LED_MAX_CURRENT		0X5D
#define R_CAIC_RGB_LED_CURRENT		0X5F
//MISSING IMAGE PROCESSING CONTROL

//GENERAL SETUP
#define W_BORDER_COLOR				0XB2
#define R_BORDER_COLOR				0X83
#define W_KEYSTONE_ANGLE			0XBB
#define R_KEYSTONE_ANGLE			0XBC

//ADMIN STATUS CONTROL
#define R_SHORT_STATUS				0XD0
#define R_SYSTEM_STATUS				0XD1
#define R_SYSTEM_SOFTWARE_VERSION	0XD2
#define R_COMM_STATUS				0XD3
#define R_CONTROLLER_DEVICE_ID		0XD4
#define R_DMD_DEVICE_ID				0XD5
#define R_FLASH_BUILD_VERSION		0XD9






//Initialize I2C, Ports, and Other peripherals for DPP3438
void dpp_init(void)
{
	//Set host IRQ as input and ProjOn as output
	P2DIR &= ~HOST_IRQ_PIN; //HOST IRQ

	P2DIR |= PROJ_ON_PIN;
	P2SEL &= ~(PROJ_ON_PIN); //Change from Crystal
	P2OUT &= ~PROJ_ON_PIN; //Proj Off

	//(Eric) Let's initialize i2c here!
	i2c_masterInit();
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

	//Red
	dpp_dispCurtain(1, 1);
	//I2C can start immediately after HOST_IRQ goes low

}

//Turn off DPP3438, 1.8V power, and I2C
void dpp_turnOff(void)
{
	//TODO: Stop I2C
	P2OUT &= ~PROJ_ON_PIN;

	//timer_waitMilli(500);
	//TODO:Turn off 1.8V Power

}


//Send write command to DPP3438
//Subaddress, Pointer to data and length of data array
void dpp_writePolledCommand(uint8_t subaddress, uint8_t *data, uint8_t numBytes)
{
	int i = 0;
	static uint8_t allData[12]; //Max param size is 12bytes

	if(numBytes > 12) return; //Error

	allData[0] = subaddress; //First element

	if(numBytes != 0)
	{
		for(i=1; i <= numBytes; i++) //First element already placed
		{
			allData[i] = data[(i-1)];
		}
	}

	if(i2c_sendPolledData(DPP_ADDR, allData, numBytes + 1))
	{
		//I2C Fault
		P3OUT |= BIT4;
	}
}

//Change splash screen
void dpp_changeSplashScreen(uint8_t imageNum)
{
	static uint8_t num = 0;
	static uint8_t flags =  I2C_UNSENT | I2C_TX_MODE;

	//dpp_writePolledCommand(W_SPLASH_SCREEN_SELECT, &num, 1);
	if(num != imageNum && !(flags & I2C_IN_BUFFER) )
	{
		num = imageNum;
		i2c_addToQueue(DPP_ADDR, W_SPLASH_SCREEN_SELECT, &num, 1, &flags );
		dpp_sourceSelect(0x02);
		dpp_splashScreenExecute();
	}
}



/**
 * Selec image source
 * @param sourceNum number of the source
 *
 */
void dpp_sourceSelect(uint8_t sourceNum)
{
	static uint8_t num = 0;
	static uint8_t flags =  I2C_UNSENT | I2C_TX_MODE;
	//dpp_writePolledCommand(W_INPUT_SOURCE_SELECT, &num, 1);
	if(!(flags & I2C_IN_BUFFER))
	{
		num = sourceNum;
		i2c_addToQueue(DPP_ADDR, W_INPUT_SOURCE_SELECT, &num, 1, &flags );
	}


}

//Display curtain
void dpp_dispCurtain(uint8_t color, uint8_t enable)
{
	/*
	 * 0: Black
	 * 1: red
	 * 2: Green
	 * 3: Blue
	 * 4: Cyan
	 * 5: Magenta
	 * 6: Yellow
	 * 7: White
	 *
	 */

	static uint8_t theColor = 0;
	static uint8_t flags = I2C_UNSENT | I2C_TX_MODE;

	if(color < 7)
	{
		if(enable)theColor = (color<<1) | 0x01;
		else theColor = (color<<1) ;

		//static uint8_t num = 0;

		//dpp_writePolledCommand(W_INPUT_SOURCE_SELECT, &num, 1);
		if(!(flags & I2C_IN_BUFFER))
		{
			i2c_addToQueue(DPP_ADDR, W_DISPLAY_IMAGE_CURTAIN, &theColor, 1, &flags );
		}
		//dpp_writePolledCommand(W_DISPLAY_IMAGE_CURTAIN, &theColor, 1);
		//return 0;

	}//else //return; //invalid color

}

//Execute splash screen
void dpp_splashScreenExecute(void)
{
	dpp_writePolledCommand(W_SPLASH_SCREEN_EXECUTE, 0x00, 0);
}



