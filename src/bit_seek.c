#include "stc.h"
#include "bit_seek.h"
//


//char bit_seek_read(char dir, char * param_ptr);
//char bit_seek_save(char dir, char * param_ptr);
#define FWD			0
#define REV			1
#define AUTO		0
#define MANU		1
#define V1_8		0
#define V3_3		1
#define VCC_SEL		0x00
#define MODE_SEL	0x200
#define USR_ICDB	0x400
#define param_1		0xfe00
#define param_2		0xffff
char iap_read(int adr)
{
	char c;
	IAP_CONTR = 0x80;
	IAP_CMD = 0x01;
	IAP_TPS = 48;
	IAP_ADDRH = adr >> 8;
	IAP_ADDRL = (char)adr;
	IAP_TRIG = 0x5a;
	IAP_TRIG = 0xa5;
	_nop_();
	c = IAP_DATA;
	IAP_CONTR = 0;
	IAP_CMD = 0;
	IAP_TPS = 0;
	IAP_ADDRH = 0;
	IAP_ADDRL = 0;
	return c;
}
void iap_write(int adr, char dat)
{
	IAP_CONTR = 0x80;
	IAP_CMD = 0x02;
	IAP_TPS = 48;
	IAP_DATA = dat;
	IAP_ADDRH = adr >> 8;
	IAP_ADDRL = (char)adr;
	IAP_TRIG = 0x5a;
	IAP_TRIG = 0xa5;
	_nop_();
	IAP_CONTR = 0;
	IAP_CMD = 0;
	IAP_TPS = 0;
	IAP_ADDRH = 0;
	IAP_ADDRL = 0;
}
void iap_erase(int adr)
{
	IAP_CONTR = 0x80;
	IAP_CMD = 0x03;
	IAP_TPS = 48;
	IAP_ADDRH = adr >> 8;
	IAP_ADDRL = (char)adr;
	IAP_TRIG = 0x5a;
	IAP_TRIG = 0xa5;
	_nop_();
	IAP_CONTR = 0;
	IAP_CMD = 0;
	IAP_TPS = 48;
	IAP_ADDRH = 0;
	IAP_ADDRL = 0;
}
char vcc_manual_save(char mode, char val)
{
	char val_1 = bit_seek_read(FWD, param_1);
	char val_2 = bit_seek_read(REV, param_2);
	char tmpval;
	if(!(mode || val_1))
		return 0;// mode AUTO, not need write
	if(mode != val_1)
		tmpval = bit_seek_save(FWD, param_1);
	if(!mode && val_1 && tmpval != 0xff)
		return 0;
	if(val != val_2)
		tmpval |= bit_seek_save(REV, param_2);
	if(0xff == tmpval)
	{
		iap_erase(VCC_SEL);
		bit_seek_save(FWD, param_1);
		if(val)
			bit_seek_save(REV, param_2);
		return 1;
	}
	return 0;
}