/*
 * main.c
 */


#include <msp430f2274.h>
#include <stdint.h>

#include "util.h"
#include "timer.h"
#include "dpp3438.h"
#include "button.h"
#include "I2C.h"

powerState_t power_state = OFF;



int main(void) {

	WDTCTL = WDTPW | WDTHOLD;
	// Stop watchdog timer
	uint8_t i2c_status = I2C_PASS;
	int toggle = 0;

    util_initClock();	//Initialize sys
    timer_initA0(); //Timer for waitMilli / Micro
    dpp_init();
    button_initPorts();



    P3DIR |= BIT4 | BIT5 | BIT6;
    P3OUT |= BIT4;




    while(1)
    {

    	button_checkPowerKey(&power_state);
		P3OUT ^= BIT6;
		timer_waitMilli(500);

		switch(power_state)
		{
		case TURN_ON:
			dpp_turnOn();
			break;
		case ON:
			//dpp_sourceSelect(1);
			timer_waitMilli(750);
			dpp_dispCurtain(1, toggle );
			toggle ^= 1;
			break;

		case TURN_OFF:
			dpp_turnOff();
			break;

		}



		/*

		if(power_state == ON)
    	{

    		dpp_sourceSelect(1);
    		timer_waitMilli(750);
			//if(i2c_status != I2C_NO_ACK) i2c_status = i2c_sendPolledData(DPP_ADDR, SplashData, 2); //Write Splash Screen
			//timer_waitMicro(100);
			//if(i2c_status != I2C_NO_ACK) i2c_status = i2c_sendPolledData(DPP_ADDR, sourceSelect, 2);
			//timer_waitMicro(100);
			//if(i2c_status != I2C_NO_ACK) i2c_status = i2c_sendPolledData(DPP_ADDR, splashScreenExecute, 1);
			//timer_waitMicro(100);


    		//dpp_changeSplashScreen(1);
    		//dpp_sourceSelect(2);

			if(i2c_status == I2C_NO_ACK)
				{
				i2c_rxNack(&i2c_status);
				P3OUT ^= BIT5;
				}

    	}*/


    	//timer_waitMicro(100);
    }
	return 0;
}
