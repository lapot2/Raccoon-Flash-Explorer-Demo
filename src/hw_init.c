#include "rfe_params.h"
#include "hw_init.h"
#include "port_mode_switch.h"

//
void sys_init()
{
	{
		P5PU = 0x0F; P5 = 0x1C;
		P35 = 1;
	}
	port_cfg(PIN_DEFAULT, SPI);
	P1SR = 0x00;// P1 SLEW
	P1DR = 0x00;// P1 DRV
	P5SR = 0xEF; // P5.4 SLEW
	P5DR = 0xEF; // P5.4 DRV
	
	PU_EXT_OFF;
}
//
void port_reinit(BYTE mode)
{
	if(manual_setup)
	{
		port_cfg(PIN_DEFAULT, SPI);
		switch((BYTE)V18)
		{
			case 0:
			{
				if(SPI == mode) 
					port_cfg(FPIN_1, PIN_QUASI|(WORD)mode);
				port_cfg(FPIN_5, PIN_QUASI|(WORD)mode);
				port_cfg(FPIN_6, PIN_QUASI|(WORD)mode);
				break;
			}
			case 1:
			{
				if(SPI == mode) 
					port_cfg(FPIN_1, PIN_QUASI|(WORD)mode);
				port_cfg(FPIN_5, PIN_QUASI|(WORD)mode);
				port_cfg(FPIN_6, PIN_QUASI|(WORD)mode);
				break;
			}
		}
	}
}
//
void port_init()
{
	pin_test = 0x00ff;
	    P1NCS = 0x00;// P1 SCHMITT TRIG
	    P2NCS = 0x00;// P2 SCHMITT TRIG
			P3NCS = 0x00;// P3 SCHMITT TRIG
			
	if(0xff00 > pin_test)
	{
		P1 = 0xFF;
		PIN_1 = 1;
	}
	else
	{
		vcc_set(OFF);
		PIN_1 = 1;
		PIN_2 = 1;
		PIN_3 = 1;
		PIN_5 = 1;
		PIN_6 = 1;
		PIN_7 = 1;
	}
}
//
void Timer0_Init(){		//32768us@24.000MHz
	AUXR &= 0x7F;			//Timer clock is 12T mode
	TMOD &= 0xF0;			//Set timer work mode
	TMOD |= 0x01;			//Set timer work mode
}
//
void Timer4_Init(){		//32768us@24.000MHz
	T4T3M &= 0xDF;			//Timer clock is 12T mode
	T4L = 0x00;				//Initial timer value
	T4H = 0x00;				//Initial timer value
	T4T3M |= 0x80;			//Timer4 start run
}
//
void RTC_init(){
	IRC32KCR = 0x80;//start internal 32khz osc for rtc
	while(!(IRC32KCR & 0x01));
	RTCCFG = 0x02;//set internal osc
}
//
void adc_init()
{
	ADCCFG |= 0x20;
	ADCEXCFG = AVERAGE16;
	ADC_CONTR = 0x80;
}
//
void comp_init()//INT 21 / priority - PCMP, PCMPH
{
	CMPCR1 = COMP_ENABLE;
	CMPCR2 = 0x40;//0x10;//0x50;//0x3F;//Filter
	CMPEXCFG = 0x40;//Hystheresis: 0x40-10mv, 0x80-20mv, 0xC0-30mv
	IP2 |= 0x20;
	IP2H |= 0x20;
	CMPCR1 &= ~COMP_IF;
}
//