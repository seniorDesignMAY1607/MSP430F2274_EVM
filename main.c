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

	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	

    util_initClock();	//Initialize sys
    timer_initA0(); //Timer for waitMilli / Micro
    dpp_init();
    button_initPorts();
    i2c_init();

    uint8_t SplashData[2] = {0x0D, 0x01}; //Splash screen data
    uint8_t sourceSelect[2] = {0x05, 0x02};
    uint8_t splashScreenExecute[1] = {0x35};

    P3DIR |= BIT4 | BIT5 | BIT6;
    P3OUT |= BIT4;




    while(1)
    {

    	button_checkPowerKey(&power_state);
		P3OUT ^= BIT6;
		timer_waitMilli(500);

    	if(power_state == ON)
    	{

			i2c_sendData(DPP_WRITE_ADDR, SplashData, 2); //Write Splash Screen
			timer_waitMilli(1);
			i2c_sendData(DPP_WRITE_ADDR, sourceSelect, 2);
			timer_waitMilli(1);
			i2c_sendData(DPP_WRITE_ADDR, splashScreenExecute, 1);

    	}


    	//timer_waitMicro(100);
    }
	return 0;
}
