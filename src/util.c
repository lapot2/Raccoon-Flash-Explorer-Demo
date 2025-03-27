/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/* --- Web: www.STCMCUDATA.com  ---------------------------------------*/
/* --- BBS: www.STCAIMCU.com  -----------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序        */
/*---------------------------------------------------------------------*/

#include "stc.h"
#include "util.h"
#include "pin_checker.h"
#include "scsi.h"


BYTE blink_tmr = 0;

DWORD reverse4(DWORD d)
{
    DWORD ret;
    
    ((BYTE *)&ret)[3] = ((BYTE *)&d)[0];
    ((BYTE *)&ret)[2] = ((BYTE *)&d)[1];
    ((BYTE *)&ret)[1] = ((BYTE *)&d)[2];
    ((BYTE *)&ret)[0] = ((BYTE *)&d)[3];

    return ret;
}
   
WORD reverse2(WORD w)
{
    WORD ret;
    
    ((BYTE *)&ret)[1] = ((BYTE *)&w)[0];
    ((BYTE *)&ret)[0] = ((BYTE *)&w)[1];

    return ret;
}
//
void blinker(BYTE count, BYTE speed)
{
	//
//	blink_tmr = count;
//	TL1 = 0x00;				//Initial timer value
//	TH1 = 0x00;				//Initial timer value
//	TF1 = 0;				//Clear TF1 flag
//	TR1 = 1;				//Timer1 start run
	//
	blink_tmr = count;
	T4L = 0x00;				//Initial timer value
	T4H = 0x00;				//Initial timer value
	//T4T3M |= 0x80;			//Timer4 start run
	IE2 |= 0x40;			//Enable timer4 interrupt
	TM4PS = speed;
}
//
void delay(WORD time)
{
	time *=2;
	time = 0xffff - time;
	TL0 = (BYTE)(time & 0x00ff);//Initial timer value
	TH0 = (BYTE)((time >> 8) & 0x00ff);//Initial timer value
	TF0 = 0;				//Clear TF0 flag
	TR0 = 1;				//Timer0 run
}
//
char xdata * uint2str(char xdata * buf_addr, long var)// compact
{
	 //int i, sign;
	char c[20];
	data char i = 0;

 //if ((sign = n) < 0) n = -n;
	
	do {c[i++] = var % 10 + '0';} while ((var /= 10) > 0);
	 //if (sign < 0) s[i++] = '-';
	
	while(i--) *buf_addr++ = c[i]; 
	
	return buf_addr;
}
//
char xdata * text(BYTE xdata * out, char text[], char len)
{
	if(!len) len = strlen(text);
	//
//	memcpy(out, text, len);
//	return out + len;
	return fast_memcpy(out, text, len, OUT_FWD|IN_FWD);
}

//



