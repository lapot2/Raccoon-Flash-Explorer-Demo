
#include "errors_collector.h"

//
char error_collector[MAX_ERROR * LEN_ERROR + 2] = {0};

xdata struct error * errorlog = error_collector;

///////// MALLOC USES TOO MUCH MEMORY, USING STATIC ARRAY

uint8_t error_collect(char code * err_label1, char err_param1, char code * err_label2, char err_param2, char code * err_label3, char err_param3)
{
	data uint8_t errorcount = 1;
	struct error * ptr = errorlog;
	
	//////////////////// Converted to array, ptr->next not needed
//	while(ptr->next)
	while(ptr->label1)
	{
		errorcount++;
		ptr++;// less than ptr = ptr->next;
//		if(errorcount > MAX_ERROR) return errorcount;// 2 byte less & slowest than out of while loop
	}

	if(errorcount > MAX_ERROR) return errorcount;
	if(ptr->label1) return errorcount;
	ptr->label1 = err_label1;
	ptr->param1 = err_param1;
	ptr->label2 = err_label2;
	ptr->param2 = err_param2;
	ptr->label3 = err_label3;
	ptr->param3 = err_param3;
	
//	if(MAX_ERROR == errorcount) return errorcount;
//	ptr->next = ptr + 1;// node ptr + 1

	return errorcount;
}
//
void error_clear()
{
	data char i;
	for(i = 0; i < MAX_ERROR * LEN_ERROR + 2; i++)
	{
		error_collector[i] = 0;
	}
//	memset_xram(error_collector, 0x00, (int)(MAX_ERROR * LEN_ERROR));// fuck????????????????????
}