#ifndef __HW_MONITOR_H__
#define __HW_MONITOR_H__

#include "hw_init.h"

extern BOOL overload;

WORD * adc_dma();//WORD * buffer);

char vcc_set(BYTE level);

void HWRESET();

WORD adc_read(BYTE channel); //reentrant;

#endif //__HW_MONITOR_H__