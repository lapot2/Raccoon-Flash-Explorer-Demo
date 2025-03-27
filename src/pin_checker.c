
#include "stc.h"
#include "util.h"
#include "spi.h"
#include "pin_checker.h"
#include "hw_monitor.h"
#include "port_mode_switch.h"

/*
PORT:	PIN: |25	| 93 | 24
P54			1: #CS		CS	 A0
P14			2:	MISO	CLK  A1
P16			3: #HOLD	DI	 A2
P11			4:	GND		DO	 GND
P13			5:	MOSI	GND	 SDA
P15			6:	SCLK	ORG  SCL
P17			7: #WP		NC	 WP
P10			8:	VCC		VCC	 VCC

ENABLE GND			P50
ENABLE VCC			P53
ENABLE V18			P52
*/

//#define PIN_1		P54 ch2
//#define PIN_2		P14 ch4
//#define PIN_3		P16 ch6
//#define PIN_4		P11 ch1
//#define PIN_5		P13 ch3
//#define PIN_6		P15 ch5
//#define PIN_7		P17 ch7
//#define PIN_8		P10 ch0

WORD xdata pin_test = 0;

void dly_us(WORD us)
{
	delay(us);
	while(!TF0);
}
//
typedef struct{
	WORD PIN8;
	WORD PIN4;
	WORD PIN1;
	WORD PIN5;
	WORD PIN2;
	WORD PIN6;
	WORD PIN3;
	WORD PIN7;
} * SOCKET;

WORD xdata pin_level_hi[8] = {0};
WORD xdata pin_level_lo[8] = {0};

//#define pintest_debug

WORD ic_check()// dummy
{
	return 0xff;
}

//
void pin_lvl_det()// dummy
{
	return;
}
//
