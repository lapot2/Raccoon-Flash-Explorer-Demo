#ifndef __HW_INIT_H__
#define __HW_INIT_H__

#include "stc.h"
#include "adc_defs.h"
#include "cmp_defs.h"
#include "power_control_defs.h"
#include "rtc_defs.h"
#include "pin_checker.h"
#include "hw_monitor.h"


void adc_init();
void comp_init();
void sys_init();
void RTC_init();
void port_init();
void port_reinit(BYTE mode);
void Timer0_Init();
void Timer1_Init();
void Timer4_Init();

#endif //__HW_INIT_H__