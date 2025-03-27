
/***********  Raccoon flash explorer Demo   ************



*******************************************************/

#include "main.h"
#include "stc.h"
#include "usb.h"
#include "usb_desc.h"
#include "scsi.h"
#include "memory.h"
#include "util.h"
#include "spi.h"
#include "i2c.h"
#include "pin_checker.h"
#include "hw_monitor.h"


#define FP1 (char)0x01
#define FP2 (char)0x02
#define FP3 (char)0x04
#define FP4 (char)0x08
#define FP5 (char)0x10
#define FP6 (char)0x20
#define FP7 (char)0x40
#define FP8 (char)0x80

char ic_types_checker(char pintest)// dummy
{
	pintest = pintest;
	return 0;
}
//
char ic_init(char allowed_ic_types)// dummy
{
	switch(allowed_ic_types){
		case 0:
		case 1: if(spi_init(SPI_PORT_1, SPI_DIV2, 0)) return 1;
		case 2: if(i2c_init(I2C_PORT_2, I2C_100K, 0)) return 1;
		case 3: break;
		default: break;
	}
	vcc_set(OFF);
	dwSectorCount = 0;
	dwMemorySize = 0;
	return 0;
}
//
void ic_detect_auto()// dummy
{
		vcc_set(VCC33); 
		ic_init(ic_types_checker((char)pin_test));
}
//
char code param_1 _at_ 0xfe00;
char code param_2 _at_ 0xffff;
char cset_fwd = 0;
char cset_rev = 0;

extern BOOL verify_enabled;
//
void ic_detect_manu()// dummy
{
}
//
void main()
{
	P_SW2 |= 0x80;  //enable XDATA
	
	sys_init();
	RTC_init();
	adc_init();
//	comp_init();
	Timer0_Init();
	Timer4_Init();
	port_init();
	EA = 1;
	//
	ic_detect_auto();
	//
	FakeFAT_init();
	usb_init();
  //
	while (1)
	{
		//
		if(0x40 == (IAP_CONTR & ~0x40)) IAP_CONTR |= 0x20;
		//
		if(overload)
		{
			blinker(255, 1);
		}
   }
}






