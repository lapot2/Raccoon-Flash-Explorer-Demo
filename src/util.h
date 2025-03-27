#ifndef __UTIL_H__
#define __UTIL_H__

//#include <stdarg.h>
#include "power_control_defs.h"

#define CRLF "\r\n"
#define endl CRLF

#define MD3U32				(*(unsigned long volatile xdata *)0xfcf0)
#define MD3U16				(*(unsigned int volatile xdata *)0xfcf0)
#define MD1U16				(*(unsigned int volatile xdata *)0xfcf2)
#define MD5U16				(*(unsigned int volatile xdata *)0xfcf4)
	//
#define LSHIFT				0x40
#define RSHIFT				0x20
#define MUL16					0x80
#define DIV16					0xA0
#define DIV32					0xC0
DWORD * MDU32(BYTE opcode, DWORD * dat);


//extern BYTE xdata serial_number[14];

extern BYTE blink_tmr;

DWORD reverse4(DWORD d);
WORD reverse2(WORD w);

void blinker(BYTE count, BYTE speed);

void delay(WORD time);

#define TRIM_ZEROES		0x80	// Trim left trailing zeroes
//char xdata * hex2str(BYTE xdata * out, BYTE * dat, BYTE len);// compact;
extern char xdata * hex2str(BYTE xdata * out, BYTE * dat, BYTE len);

#define OUT_FWD			0
#define OUT_REV			0x80
#define IN_FWD			0
#define IN_REV			0x40
//WORD * fast_memcpy(char * out, char * inp, WORD len, char mode);
BYTE xdata * fast_memcpy(BYTE xdata * out, char * inp, WORD len, char mode);

extern void * memset_xram(char xdata * addr, char dat, int len);//ugly

char xdata * uint2str(char xdata * buf_addr, long var);// compact;

char xdata * text(BYTE xdata * out, char text[], char len);

///////////////////////////////////////////////// RTC ALARM TIMER

void rtc_alarm_set();
void rtc_alarm_delay(char delay);

#endif
