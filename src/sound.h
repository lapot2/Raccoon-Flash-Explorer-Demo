
//#include "main.h"
#include "stc.h"
//#include "usb.h"
//#include "usb_desc.h"
//#include "scsi.h"
#include "util.h"
//#include "memory.h"
//#include "spi.h"
//#include "i2c.h"
//#include "pin_checker.h"
//#include "hw_monitor.h"
//#include "scsi_rw_parser.h"
//#include "build.h"//*** ERROR L104: MULTIPLE PUBLIC DEFINITIONS//SYMBOL:  BUILD_DATE//MODULE:  .\obj\sound.obj (SOUND)

//#define SYSclk           24000000UL
#define SYSclk           44236800UL
#define Brake_PWM        PWMA_BKR &= ~0x80
#define UnBrake_PWM      PWMA_BKR |=  0x80

void PWM_setup();
void PWM_set_prescaler(uint16_t PWM_Freq, uint16_t ARR_reg);
void PWM_set_duty(int16_t duty, int16_t range);
void PWM_start();
void PWM_stop();

extern bool sound_en;
extern WORD freq;
void sound_init(WORD pwm_freq);
void beep(BYTE len_on, BYTE len_off, BYTE count);