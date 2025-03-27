#ifndef __ERRORS_COLLECTOR_H__
#define __ERRORS_COLLECTOR_H__

#include <stdlib.h>
#include "stc.h"
#include "util.h"

//#define MAX_ERROR 8 // error_collector[(MAX_ERROR + 1) * LEN_ERROR]
#define MAX_ERROR 8 // error_collector[MAX_ERROR * LEN_ERROR + 2(endian marker)]
#define LEN_ERROR sizeof(struct error)

//////////////////// Converted to array, error->next not needed
struct error{
	char code * label1;	// 2b error text label ptr
	char param1;
	char code * label2;
	char param2;
	char code * label3;
	char param3;
//	struct error xdata * next;// 2b next error ptr
}; 

extern xdata struct error * errorlog;

uint8_t error_collect(char code * err_label1, char err_param1, char code * err_label2, char err_param2, char code * err_label3, char err_param3);
void error_clear();

#endif //__ERRORS_COLLECTOR_H__