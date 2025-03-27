
#include "sound.h"

// Using PWM3_3 P64-P65
//
// Fosc maybe 24M, Freq 3.5k, Duty 50, Diff output

void PWM_setup()
{
	PWMA_PS = 0x20;//0b00200000;//P64-P65 PWM3N_3-PWM3P_3
	PWMA_CCER2 = 0x00;
	PWMA_CCMR3 = 0x60;//OC3M 1:1:0 0b01100000;
	PWMA_CCER2 = 0x05;//CC3NE CC3E 0b00000101
//            PWMA_OISR  = 0b00000000;
//            PWMA_PSCRH = 0x00;///< Prescaler High
//            PWMA_PSCRL = 0x08;///< Prescaler Low
//            PWMA_ARRH = 0x03;///< Period High
//            PWMA_ARRL = 0x84;///< Period Low
//	PWM_set_prescaler(3500, 10000);
//            PWMA_CCR4H = 0x00;///< Duty High
//            PWMA_CCR4L = 0xff;///< Duty Low
	PWMA_ENO = 0x30;//ENO3N ENO3P
	PWMA_BKR = 0x04;//OSSIA 0b00000100;
	PWMA_CR1 = 0x01;//CENA
	
//	P6SR = 0xcf; 
//  P6DR = 0xcf; 
}

void PWM_set_prescaler(uint16_t PWM_Freq, uint16_t ARR_reg)
{
    uint16_t PSCR_val;
//    PWM_set_duty(50, 100);
    PSCR_val   = SYSclk / (PWM_Freq * ((uint32_t)ARR_reg+1)) - 1;
    if(PSCR_val >= UINT_MAX)
        PSCR_val = UINT_MAX;
    PWMA_PSCRH = PSCR_val >> 8;
    PWMA_PSCRL = PSCR_val % 256;
    PWMA_ARRH  = ARR_reg >> 8;
    PWMA_ARRL  = ARR_reg % 256;
		PWM_set_duty(50, 100);
//		PWM_set_duty(500, 1000);
}

void PWM_set_duty(int16_t duty, int16_t range)
{
    ///< code
    uint16_t ARR_reg, CCRx_reg;
    if(duty >= range)
        duty = range;
    if(duty <= 0)
        duty = 0;
    ARR_reg  = (PWMA_ARRH << 8) | PWMA_ARRL;
    CCRx_reg = (((uint32_t)ARR_reg+1) * duty) / range;
    PWMA_CCR3H = CCRx_reg >> 8;
    PWMA_CCR3L = CCRx_reg % 256;
//    return CCRx_reg;
}
//
//void PWM_start()
//{
//	UnBrake_PWM;
//}
////
//void PWM_stop()
//{
//	Brake_PWM;
//}
//
bool sound_en = 1;
WORD freq = 500;
void sound_init(WORD pwm_freq)
{
	if(pwm_freq) freq = pwm_freq;
	PWM_setup();
	PWM_set_prescaler(freq, 100);
}
//
void beep(BYTE len_on, BYTE len_off, BYTE count)// :D
{
//	PWM_set_prescaler(freq, 100);
	while(count--)
	{
		UnBrake_PWM;
	//	PWM_start();
		while(len_on--)
		{
			delay(30000);while(!TF0);
		}
		Brake_PWM;
	//	PWM_stop();
		while(len_off--)
		{
			delay(30000);while(!TF0);
		}
	}
	Brake_PWM;
}

//
/*void song()// :D
{
	WORD i;
	UnBrake_PWM;
	for(i = 2000; i; i+= 20)
	{
		PWM_set_prescaler(i, 100);delay(30000);while(!TF0);
		if(i < 2000) i = 2000;
		if(i > 6000) i = 2000;
	}
	Brake_PWM;
}*/
//
/*void hlsound()// Half-Life die sound :D
{
	// Half-Life die sound :D
	freq = 4100;
	beep(5,5,1);beep(3,10,1);beep(5,5,1);beep(3,10,1);
	beep(10,15,1);beep(10,15,1);
	beep(70,0,1);
	while(1);
}*/
//
/*void sound()
{
	WORD i;
	PWM_setup();
//	PWM_set_prescaler(freq, 100);
	//sound test
	//	song(); while(1);// :D
//	hlsound();// Half-Life die sound :D
	
	freq = 4100;PWM_set_prescaler(freq, 100);return;//1450/1950/4000/4100/5000
	new_freq:
	PWM_set_prescaler(freq, 100);
//	PWM_set_duty(500, 1000);
	UnBrake_PWM;
//	PWM_start();
	PIN_2 = 1; 
	PIN_3 = 1;
	for(i = 0; i < 10; i++) {delay(30000);while(!TF0);}
	
	while(PIN_5)
	{
		if(!PIN_2)
		{
			PIN_2 = 0;
			freq += 50;
			if(freq > 6000) freq = 500;
			goto new_freq;
		}
		else if(!PIN_3)
		{
			PIN_3 = 0;
			freq -= 50;
			if(freq < 500) freq = 6000;
			goto new_freq;
		}
	}
	Brake_PWM;
//	PWM_stop();
	PIN_2 = 1; 
	PIN_3 = 1;
}*/