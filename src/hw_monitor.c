#include "main.h"
#include "stc.h"
#include "util.h"
#include "scsi.h"
#include "pin_checker.h"
#include "hw_monitor.h"
#include "port_mode_switch.h"

BOOL overload = 0;

WORD * adc_dma()// dummy
{
	return nullptr;
}


//
void vcc_switch(BYTE mode)
{
	if(VCC33 == mode)// int pu on / push-pull / 3.3v
	{
		//
		GND_ON;
		PU_EXT_ON;
		P1PU = 0xff;
		P5PU |= 0x10;
		V18_OFF;
		VCC_ON;
	}
	else if(OFF == mode)// all off / vcc off
	{
		VCC_OFF;
		PU_EXT_OFF;
		P1PU = 0x00;
		P5PU &= ~0x10;
		GND_OFF;
	}
}
//
char vcc_set(BYTE level)// dummy
{
	if(!(CMPCR1 & COMP_ENABLE)) 
	{
		vcc_switch(level); 
		delay(30000); while(!TF0);
		delay(30000); while(!TF0);
		delay(30000); while(!TF0);
		delay(30000); while(!TF0);
		delay(30000); while(!TF0);
		delay(30000); while(!TF0);
	}
	return 0;
}
//
void HWRESET()
{
	IAP_CONTR |= 0x40;//set flag for reset trig
}
static double xdata ref;//, ddat;
WORD adc_read(BYTE channel) //reentrant// dummy
{
	channel = channel;
 return 0;
}
