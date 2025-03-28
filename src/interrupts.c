#include "interrupts.h"

//
void Timer4_Isr() interrupt 20{
		if(blink_tmr)
	{
		if(255 > blink_tmr)
			blink_tmr--;
		AUXINTIF &= ~0x04;				//Clear TF4 flag
		P70 = ~P70;
	}
	else
	{
		IE2 &= ~0x40;			//Disable timer4 interrupt
		P70 = 1;
	}
}
//
void comp_isr() interrupt 21// dummy
{
}
//
