#ifndef __PORT_MODE_SWITCH_H__
#define __PORT_MODE_SWITCH_H__

#include "stc.h"
#include "util.h"
#include "scsi.h"

//#define OFF					(char)0x00
#define SPI     		(char)0x10
#define I2C     		(char)0x20
#define E93					(char)0x30
//#define E25					(char)0x40
//#define E95					(char)0x50

#define FPIN_2_E93   (char)0x32
#define FPIN_3_E93   (char)0x34
#define FPIN_4_E93   (char)0x33

#define FPIN_5_I2C   (char)0x24
#define FPIN_6_I2C   (char)0x25

#define FPIN_GND     (char)0x50
#define FPIN_GND93   (char)0x51
#define FPIN_V18     (char)0x52
#define FPIN_VCC     (char)0x53
#define FPIN_LED     (char)0x70

#define FPIN_1   		(char)0x54
#define FPIN_2   		(char)0x14
#define FPIN_3   		(char)0x16
#define FPIN_4   		(char)0x11
#define FPIN_5   		(char)0x13
#define FPIN_6   		(char)0x15
#define FPIN_7   		(char)0x17
#define FPIN_8   		(char)0x10
#define PIN_NUM   	(char)0x80
#define PIN_DEFAULT (char)0xff

//					       PxM0:PxM1
#define PIN_QUASI		(WORD)0x0000
#define PIN_OPDRN		(WORD)0x0101
#define PIN_PPULL		(WORD)0x0100
#define PIN_INPUT		(WORD)0x0001

#define PIN_INIT		(WORD)0x0000

typedef struct{
	char PPU0;
	char PPU1;
	char PPU2;
	char PPU3;
	char PPU4;
	char PPU5;
	char PPU6;
	char PPU7;
	}PORT_PULL_UP;

typedef struct{
	char PNCS0;
	char PNCS1;
	char PNCS2;
	char PNCS3;
	char PNCS4;
	char PNCS5;
	char PNCS6;
	char PNCS7;
	}PORT_SCHMIDT_TRIG;

typedef struct{
	char PSR0;
	char PSR1;
	char PSR2;
	char PSR3;
	char PSR4;
	char PSR5;
	char PSR6;
	char PSR7;
	}PORT_SLEW_RATE;

typedef struct{
	char PDR0;
	char PDR1;
	char PDR2;
	char PDR3;
	char PDR4;
	char PDR5;
	char PDR6;
	char PDR7;
	}PORT_CURRENT;

typedef struct{
	char PIE0;
	char PIE1;
	char PIE2;
	char PIE3;
	char PIE4;
	char PIE5;
	char PIE6;
	char PIE7;
	}PORT_INPUT_ENABLE;

void port_cfg(char ic_pin, uint16_t pinmode);

#endif//__PORT_MODE_SWITCH_H__
