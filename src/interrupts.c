#include "interrupts.h"
//#include "sound.h"
//
/*void Timer1_Isr() interrupt 3{
	if(blink_tmr)
	{
		if(255 > blink_tmr)
			blink_tmr--;
		//TL1 = 0x00;				//Initial timer value
		//TH1 = 0x00;				//Initial timer value
		TF1 = 0;				//Clear TF1 flag
		//TR1 = 1;				//Timer1 start run
		P70 = ~P70;
	}
	else
	{
		P70 = 1;
	}
}*/
//
/*void ext4_isr() interrupt 16{// usb fucked test
	char i, tmr = 255;
	if((AUXINTIF & 0x40))
	{
		AUXINTIF &= ~0x40;
	}
	//delay(100);
	//while(!TF0);
	//
	if(!P30 || !P31)
	{
		P3PU =  0x03;
		//P3M0 = 0x03;
		//P3M1 = 0xFC;
		//P3DR = 0xFC;
		//P3SR = 0xFC;
		//for(i = 0; 10 > i; i++)
		//{
			tmr = 255;
			while(tmr)
			{
				delay(1000);
				while(!TF0);
				tmr--;
			}
		//}
		if(!P30 || !P31)
		{
			goto port_dead;
		}
		else
			goto port_ok;
	}
	else
	{
		goto port_dead;
	}
	//
	port_ok:
	//P7PU = 0x00;
	P3M0 = 0x00;
	P3PU =  0x00;
		P3DR = 0xFF;
		P3SR = 0xFF;
	for(i = 0; 10 > i; i++)
	{
		tmr = 255;
		while(tmr)
		{
			delay(500);
			while(!TF0);
			tmr--;
		}
		//P7PU = !(P7PU & 0x01);
		P70 = ~P70;
	}
	return;
	//
	port_dead:
	P1M0 = 0x03;
	P1M1 = 0xFC;
	P1DR = 0xFC;
	//P11 = 0;
	P3M0 = 0x00;
	P3PU =  0x00;
		P3DR = 0xFF;
		P3SR = 0xFF;
	//P5M1 = 0xF2;
	P70 = ~P70;
	//P52 = 0;
	for(i = 0; 4 > i; i++)
	{
		tmr = 255;
		while(tmr)
		{
			delay(500);
			while(!TF0);
			tmr--;
		}
		//P50 = ~P50;
		//P53 = ~P53;
		P11 = ~P11;
	}
	P11 = 1;
	P1DR = 0xFF;
	P1M1 = 0xFF;
	P1M0 = 0x00;
	//P52 = 1;
	//P5M1 = 0xFF;
	P70 = ~P70;
	return;
}*/
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
//	if(!P70 && sound_en) UnBrake_PWM;
//	else Brake_PWM;
//	if(!P70) PWM_start();
//	else PWM_stop();
}
//
void comp_isr() interrupt 21
{
//	if(CMPCR1 & COMP_PIE)
//	{
////		CMPCR1 &= ~COMP_PIE;		CMPCR1 |= COMP_NIE;		CMPCR1 &= ~COMP_IF;
//		CMPEXCFG = 0x40;
//		CMPCR1 = 0x90;
//		return;
//	}
//	if(CMPCR1 & COMP_NIE)
//	{
//		//
//	}
	//
	CMPCR1 &= ~COMP_IF;
	//return;
	if(!VCC)// & !(CMPCR1 & COMP_PIE))
	{
		//CMPCR1 &= ~COMP_IF;
		PU_EXT_OFF;
		VCC_OFF;
		GND_OFF;
//		vcc_set(OFF);
		PIN_1 = 1;
		PIN_2 = 1;
		PIN_3 = 1;
		PIN_5 = 1;
		PIN_6 = 1;
		PIN_7 = 1;
		overload = 1;
		scsi_ready = 0;
	}
}
//
