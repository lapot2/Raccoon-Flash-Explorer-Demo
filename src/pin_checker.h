#ifndef __PIN_CHECKER_H__
#define __PIN_CHECKER_H__

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
#define ON			1
#define OFF			0

#define PIN_1			P54
#define PIN_2			P14
#define PIN_3			P16
#define PIN_4			P11
#define PIN_5			P13
#define PIN_6			P15
#define PIN_7			P17
#define PIN_8			P10



extern WORD xdata pin_test;

extern WORD xdata pin_level_hi[8];
extern WORD xdata pin_level_lo[8];

WORD ic_check();
void pin_lvl_det();

#endif //__PIN_CHECKER_H__