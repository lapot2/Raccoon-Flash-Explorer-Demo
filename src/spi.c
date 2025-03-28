#include "stc.h"
#include "usb.h"
#include "util.h"
#include "scsi.h"
#include "scsi_rw_parser.h"
#include "memory.h"
#include "spi.h"
#include "pin_checker.h"
#include "hw_monitor.h"
#include "port_mode_switch.h"
#include "rfe_params.h"
#include "sfdp.h"
//#include <stdlib.h>


WORD spi_ver_err = 0;

BYTE xdata spi_id_arr[16] = {0};
BYTE xdata * spi_RDID_id = spi_id_arr + 0;// NEED 3 BYTES
BYTE xdata * spi_REMS_id = spi_id_arr + 1;// NEED 3 BYTES
BYTE xdata * spi_JDID_id = spi_id_arr + 5;// NEED 3 BYTES
BYTE xdata * spi_UUID_id = spi_id_arr + 8;// NEED 8 BYTES

BOOL sfdp_present = 0;
BOOL vcc_sfdp = 0;
BYTE xdata spi_regs[3];

void spi_disable()
{
	SPCTL &= ~SPI_ENABLE;
}
//
void spi_write_disable()
{
	SPI_SS = 0;
	spi_send(SPI_WRDI);
	SPI_SS = 1;
}
//
void spi_write_enable() small
{
    while (spi_isbusy());

    SPI_SS = 0;
    spi_send(SPI_WREN);
    SPI_SS = 1;
}

//
//*****************************************
void spi_cmd(BYTE cmd, DWORD addr) small
{
ite_reg(SPI_WREAR, *((char*)&addr), 0xff, 0xff);
	
	if(*((char*)&dwMemorySize))//if MSB > 0
	{
		spi_write_enable();
		
		set_ext_addr(SPI_WREAR, *((char*)&addr));// perm extend addr set
	}
	//
	if(SPI_READ != cmd) spi_write_enable();
	else if(!(*((char*)&dwMemorySize))) while(spi_isbusy());

	//
	SPI_SS = 0;
	spi_send(cmd);
	spi_send(*((char*)&addr+1)); spi_send(*((char*)&addr+2)); spi_send(*((char*)&addr+3));
	
//	if(cmd == SPI_CHIPER) return;//for formatting
}
//
BYTE spi_read_reg(BYTE reg)
{
	SPI_SS = 0;
		spi_send(reg);
		reg = spi_send(0);
	SPI_SS = 1;
	delay(1);
	while(!TF0);
	return reg;
}
//
// len: 0/1
//#define CONT	(char)0x00// byte array
//#define END		(char)0x01// one/last byte // defined in SPI.H
BYTE spi_write_custom_byte(char dat, bool len) small
{
	SPI_SS = 0;
	spi_send(dat);
	if(len) SPI_SS = 1;
	return SPDAT;
}
//
BYTE * spi_send_cmd(BYTE reg, BYTE dummy_cyc, BYTE xdata * regs_arr, BYTE count)
{
	SPI_SS = 0;
			spi_send(reg);
	while(dummy_cyc--) spi_send(0);
	while(count--) *regs_arr++ = spi_send(0);
	SPI_SS = 1;
	//
	return regs_arr;
}
//
//BYTE spi_write_reg(BYTE reg, BYTE dat1, BYTE dat2, BYTE dat3)
BYTE spi_write_reg(BYTE reg, BYTE dat1, BYTE dat2)
{
	spi_write_enable();
		
	SPI_SS = 0;
	spi_send(reg);
	if(dat1 != 0xFF)
		spi_send(dat1);
	if(dat2 != 0xFF)
		spi_send(dat2);
	SPI_SS = 1;
	//
	//
	delay(5); while(!TF0);
	if(!spi_isbusy()) spi_write_disable();
	//
	return reg;
}
//
void spi_enable(char en);
///////////////////////////////////////////////////////////////////////////////  INIT
#define OK		0
#define FAIL	0x80
#define ID_OK				0x01
#define REGS_OK			0x02
#define VCC_OK			0x04
#define SFDP_OK			0x08
#define VER_BUS_OK	0x10
#define VER_IC_OK		0x20
#define QUAD_OK			0x40


BOOL reg_wr_test = 0;
BOOL reg_wr_flag = 0;
//
BOOL spi_init(char port, char speed, char init_level)
{
	P_SW1 = (P_SW1 & ~(3<<2)) | ((port & 0x03) << 2);
	SPCTL = 0x9c;// 0b10011100// MODE 3
	//
	speed = speed;//dummy using var
	spi_enable(1);
	port_reinit(SPI);
	spi = 1;
	SPI_SS = 1;
	SPI_WP = 1;
	SPI_HOLD = 1;
  fFlashOK = 0;
	
	if(manual_setup) init_level |= VCC_OK;
	
	if(0x80 == (spi_check(speed, init_level) & 0x80))
	{
		spi_enable(0);
		spi = 0;
		
		return 0;
	}
	else
	{
		return 1;
	}
}
BYTE idata spi_speed;
void spi_set_speed(BYTE speed) small// dummy
{
	speed = speed;
}

#define SPI_ENABLE	0x40
void spi_enable(char en)
{
	if(en)
		SPCTL |= SPI_ENABLE;
	else
		SPCTL &= ~SPI_ENABLE;
}

void spi_reset_cmd()
{
	spi_write_custom_byte(0x66, END);
	spi_write_custom_byte(0x99, END);
	delay(50);//default 50us/24M
	while(!TF0);
}
//
char spi_get_id(char speed)//ok..
{
	data BYTE i;
	data BYTE errorcount = 0x00;
	xdata char xdata * ptr = spi_id_arr;
	
	//id reader
	spi_set_speed(speed);
	spi_reset_cmd();
	
	memset(spi_id_arr, 0x00, 16);//debug

	spi_send_cmd(SPI_JDID, 0, \
	spi_send_cmd(SPI_REMS, 3, \
	spi_send_cmd(SPI_RDID, 3, spi_id_arr, 1), 4), 3);
	
	for(i = 0; i < 8; i++)
	{
		switch(*ptr++)
		{
			case 0x00: 
			case 0xff: continue;//CHECK OF ID's PRESENT
			default: errorcount |= 0x01 << i; break;
		}
		
		switch(i)
		{
			case 1:// REMS parity check
			case 5: if(spi_parity_check(spi_id_arr[i])) return FAIL;// JDID parity check
			default: break;
		}
	}

///////////////////////////////////////////////// most ugly | id replacing
	if(!errorcount) return FAIL;
	else if(0x01 == errorcount) spi_id_arr[7] = spi_id_arr[0] + 1;
	else if((errorcount & 0xe0) != 0xe0)
	{
		spi_id_arr[5] = spi_id_arr[1];
		spi_id_arr[7] = spi_id_arr[2] + 1;
	}
	return OK;
}
//
char spi_get_regs(char speed)//ok..
{
	//regs reager
	data char i = 0;
	spi_set_speed(speed);
	SPI_CS = 0;
	spi_send(SPI_RDSR1); i = spi_send(0); i &= spi_send(0);
	SPI_CS = 1;
	spi_regs[0] = i;
	
	SPI_CS = 0;
	spi_send(SPI_RDSR2); i = spi_send(0); i &= spi_send(0);
	SPI_CS = 1;
	spi_regs[1] = i;
	
	SPI_CS = 0;
	spi_send(SPI_RDSR3); i = spi_send(0); i &= spi_send(0);
	SPI_CS = 1;
	if(0xFF == i)
	{
		SPI_CS = 0;
		spi_send(SPI_RDSR4); i = spi_send(0); i &= spi_send(0);
		SPI_CS = 1;
	}
	spi_regs[2] = i;
	
	if(0xFF == spi_regs[0] && 0xFF == spi_regs[1] && 0xFF == spi_regs[2])
		return FAIL;
	else
		return OK;
}
//
#define FALSE			(bool)0
#define TRUE			(bool)1
BYTE sfdp_vcc_seek(BYTE xdata * sfdp)// dummy
{
	sfdp = sfdp;
	return OK;
}
//
char spi_get_sfdp_data(char speed)//strip
{
	//sfdp reader & parser
	BYTE data i = 0;
	BYTE xdata * ptr = scsibuf;
	DWORD xdata * ptr2 = nullptr;
	vcc_sfdp = 0;
	
//	memset(scsibuf, 0, 256);
	///////////////////////// read
	spi_set_speed(SPI_DIV16);
	//
	SPI_CS = 0;
	spi_send(SPI_SFDP);
	spi_send(0); spi_send(0); spi_send(0); spi_send(0);
	do
	{
		*ptr++ = spi_send(0);
		i++;
	}while(i);
	SPI_CS = 1;
	
	//////////////////////// verify
	spi_set_speed(speed);
	//
	i = 0;
	ptr = scsibuf;
	SPI_CS = 0;
	spi_send(SPI_SFDP);
	spi_send(0); spi_send(0); spi_send(0); spi_send(0);
	do
	{
		if(*ptr++ != spi_send(0)) {SPI_CS = 1; /*verify_err = 1;*/ verify_err_addr = i; return 0x81;}
		i++;
	}while(i);
	SPI_CS = 1;
	
	if(0xffffffff == *(DWORD *)&scsibuf) return FAIL;
	if(*(DWORD *)scsibuf != 0x53464450UL) return 0x81;
	
	sfdp_read(scsibuf, 4096);
	
	ptr2 = (DWORD *)scsibuf;
	for(i = 0; i < 64; i++)
	{
		// sfdp LE2BE
		*ptr2 = reverse4(*ptr2);
		ptr2 ++;
	}
	
	sfdp_present = 1;
	return OK;
}
//
char spi_test_verify(char speed)// dummy
{
	speed = speed;
	return 0;
}
//
char spi_test_vcc(char speed)// dummy
{
	speed = speed;
	return OK;
}
//
char spi_test_quad(char speed)//need review, ugly
{
//	char data i = SPI_QPI_EN;
	speed = speed;//dummy using var
	//quad spi detector
	if(0xd5 == spi_JDID_id[0] || spi_regs[1] == 0xFF)// why 0xff check?
//		goto skip;
		return OK;
	reg_wr_test = 1;
	spi_write_reg(SPI_WRSR2, 0x02, 0xff);
	while(spi_isbusy());
	if((spi_read_reg(SPI_RDSR2) & 0x02) != 0x02)
	{
		spi_write_reg(SPI_WRSR1, spi_regs[0], 0x02);
		while(spi_isbusy());
	}
	delay(50); while(!TF0);
	
	spi_write_custom_byte(SPI_QPI_EN, END);
	
//	spi_write_reg(SPI_QPI_EN, 0xff, 0xff);
//	if(MXIC == spi_JDID_id[0]) i = SPI_QPI_EN_MX;
//	spi_write_reg(i, 0xff, 0xff, 0xff);
			
	if(spi_isbusy())
	{
		QPI_PRESENT = 1;
		//
		SPCTL &= ~SPI_ENABLE;PIN_6 = 0;
		SPI_SS = 0;
		_nop_();
		_nop_();
		_nop_();
		_nop_();
//		PIN_6 = 0;
		PIN_6 = ~PIN_6;
		_nop_();
		_nop_();
		_nop_();
//		PIN_6 = 1;
		PIN_6 = ~PIN_6;
		_nop_();
		_nop_();
		_nop_();
//		PIN_6 = 0;
		PIN_6 = ~PIN_6;
		_nop_();
		_nop_();
		_nop_();
//		PIN_6 = 1;
//		PIN_6 = ~PIN_6;
		_nop_();
		_nop_();
		_nop_();
//		}
		PIN_6 = 1;
		SPI_SS = 1;
		SPCTL |= SPI_ENABLE;
	}
	
	if(reg_wr_test)
	{
		spi_write_reg(SPI_WRSR2, spi_regs[1], 0xff);
		while(spi_isbusy());
		if(spi_read_reg(SPI_RDSR2) != spi_regs[1])
		{
			spi_write_reg(SPI_WRSR1, spi_regs[0], spi_regs[1]);
			while(spi_isbusy());
		}
		reg_wr_test = 0;
	}
	return OK;
}
//
#define SPI_AA55_TEST
char spi_unlock()//ok..
{
 	//disable write protection
#ifdef SPI_AA55_TEST
	if(spi_regs[1] != 0xFF && spi_regs[1] & 0x01)
	{
		spi_write_custom_byte(0xAA, END);
		spi_write_custom_byte(0x55, END);
		//////////////
		spi_write_reg(SPI_WRSR1, 0x00, 0xff);
	}
	else
#endif
	{
		spi_write_custom_byte(0x50, END);
		spi_write_custom_byte(SPI_WRSR1, 0);
		spi_write_custom_byte(0, END);
	}
	spi_write_reg(SPI_GBU, 0xff, 0xff);
		spi_read_reg(SPI_RDSR1);
		spi_read_reg(SPI_RDSR2);
/////////////////////////////////// END OF LOCK AA55 TEST
	return OK;
}
//
//
char spi_check(char speed, char init_level)//in working
{
	char tmp_id[8]; char tmp = 0;

//////////////////////////////////////////////////////   first id test
	if(spi_get_id(SPI_DIV16)) return FAIL | init_level;
	//second id test
	memcpy(tmp_id, spi_id_arr, 8);
	{
		for(; speed < 4; speed++)
		{
			if(!spi_get_id(speed)) 
				{
					if(!memcmp(spi_id_arr, tmp_id, 8)) break;
				}
		}
		if(4 == speed) return FAIL | init_level;
		init_level |= ID_OK;
	}
	
//////////////////////////////////////////////////////   regs read
	if(spi_get_regs(speed)) return FAIL | init_level;
	else init_level |= REGS_OK;
	
//////////////////////////////////////////////////////   sfdp read	
spi_get_sfdp_data(speed);
	
//////////////////////////////////////////////////////   vcc detect	
// dummy
	
//////////////////////////////////////////////////////   verify & speed test	
// dummy
	
//////////////////////////////////////////////////////   quad test	
	if(!spi_test_quad(speed)) init_level |= QUAD_OK;
	
//////////////////////////////////////////////////////   write protect clear	
	spi_unlock();
	
//////////////////////////////////////////////////////   detect spi name & size
	spi_detect();
	fFlashOK = 1;//old style var
	spi_speed = 1;
	
	return init_level;
}
