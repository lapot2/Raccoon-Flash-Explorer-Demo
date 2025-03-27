#include "stc.h"
#include "usb.h"
#include "scsi.h"
#include "memory.h"
#include "util.h"
#include "i2c.h"
#include "pin_checker.h"
#include "hw_init.h"
#include "port_mode_switch.h"
//#include "i2c_size_detect.h"

#define I2C_READ				1
#define I2C_WRITE				0
#define I2C_BUSY		(0x80 & I2CMSST)
#define I2C_RDY			(0x40 & I2CMSST)
#define I2C_RDY_CLR	(I2CMSST &= ~0x40)
#define I2C_ACK_RX	(0x02 & I2CMSST)
#define I2C_ACK_TX	(I2CMSST &= ~0x01)
#define I2C_NACK_TX	(I2CMSST |= 0x01)

//#define I2C_START		do{I2CMSCR = 0x01; delay(20000); while(!I2C_RDY){if(TF0) return}; I2C_RDY_CLR;}while(0) // maybe response error fix...
#define I2C_START		do{I2CMSCR = 0x01; while(!I2C_RDY); I2C_RDY_CLR;}while(0)
#define I2C_STOP		do{I2CMSCR = 0x06; while(!I2C_RDY); I2C_RDY_CLR;}while(0)
//#define I2C_RESTART	(I2C_STOP; I2C_START;)
#define I2C_TX			do{I2CMSCR = 10; while(!I2C_RDY); I2C_RDY_CLR;}while(0)
#define I2C_RX			do{I2CMSCR = 11; while(!I2C_RDY); I2C_RDY_CLR;}while(0)
#define I2C_RX_NAK	do{I2CMSCR = 12; while(!I2C_RDY); I2C_RDY_CLR;}while(0)

static xdata enum {
	E24C00 = 0,
	E24C01 = 1,
	E24C02 = 2,
	E24C04 = 4,
	E24C08 = 8,
	E24C16 = 16,
	E24C32 = 32,
	E24C64 = 64,
	E24C128 = 128,
	E24C256 = 256,
	E24C512 = 512,
	E24C1024 = 1024,
	E24C2048 = 2048
}i2c_list;

WORD xdata i2c_type = 0;
char xdata i2c_page_size = 0;
//WORD xdata i2c_virtual_page_size = 0;
DWORD xdata i2c_size = 0;
BOOL i2c_large_size = 0;
char xdata i2c_slave_addr = 0;
//BOOL i2c_rw;


sbit i2c_A0 = P5^4;
sbit i2c_A1 = P1^4;
sbit i2c_A2 = P1^6;
sbit i2c_WP = P1^7;


//BOOL i2c_A0;
//BOOL i2c_A1;
//BOOL i2c_A2;
//BOOL i2c_WP;


//char i2c_speed = 20;//100kHz | MSSPEED | 0-63d | Fosc / 2 / (MSSPEED * (2 + 4) | 2MHz - 31.746kHz
BOOL i2c_init(char port, char i2c_speed, char init_level)
{
	//BYTE tmp;
	//
	P_SW2 = (P_SW2 & ~(3 << 4)) | ((port & 0x03) << 4);//0x30;
	I2CCFG = I2C_ENABLE | I2C_MASTER_MODE | (i2c_speed & 0x3f);//0x80 | 0x40;//0b110000000;
	I2CMSAUX = 0x01;//enable auto-tx after write tx reg
	I2C_RDY_CLR;
	fFlashOK = 0;
	
	port_cfg(PIN_DEFAULT, I2C);
	port_reinit(I2C);
	
	init_level = init_level;// dummy var
	
	i2c_A0 = 0;
	i2c_A1 = 0;
	i2c_A2 = 0;
	i2c_WP = 0;
//	P2M0 = 0x30;
//	P2IE = 0xff; 
	
	delay(100);
	while(!TF0);
	
	if(!i2c_check())
	{
		i2c_disable();
		i2c_A0 = 1;
		i2c_A1 = 1;
		i2c_A2 = 1;
		i2c_WP = 1;
		port_cfg(PIN_DEFAULT, SPI);
//		port_reinit(I2C);
//		P2M0 = 0x00;
//		P2IE = 0xcf; 

		delay(100);
		while(!TF0);
		//P11 = 0;
		return 0;
	}
	i2c = 1;
	return 1;
}


void i2c_disable()
{
	I2CCFG &= ~I2C_ENABLE;
}

BOOL i2c_send_data(BYTE dat)
{
	I2CTXD = dat;
	delay(5000);
	while(!I2C_RDY && !TF0);
	I2C_RDY_CLR;
	return (BOOL)((I2CMSST >> 1) & 0x01);//I2C_ACK_RX;
}

BYTE ee_address_size = 0;
BYTE ee_page_size = 0;
DWORD ee_segment_size = 0;
DWORD ee_total_mem = 0;

BOOL i2c_check()
{
	//char tmp = 0;
	//char i = 0;
	WORD c;
	if(!fFlashOK)
	{
		//fFlashOK = 1;
		//i2c size checker is //not// written
		I2C_START;
		if(i2c_send_data(0xa0))
		{
			I2C_STOP;
			return fFlashOK;
		}
		I2C_STOP;
		//i2c_WP = 0;
		 c =   get_address_size();
		if(2 == c)
		{
			i2c_large_size = 1;
			memcpy(memory_name, "16 bit adr ", sizeof(memory_name));
			//return 1;
		}
		else if(1 == c)
		{
			i2c_large_size = 0;
			memcpy(memory_name, "8 bit adr  ", sizeof(memory_name));
			//return 1;
		}
		else if(0xff == c)
		{
			memcpy(memory_name, "i page  err", sizeof(memory_name));
			return 0;
		}
		else if(0xfe == c && 1 == ee_address_size)
		{
			memcpy(memory_name, "i small err", sizeof(memory_name));
			return 0;
		}
		else if(0xfe == c && 2 == ee_address_size)
		{
			memcpy(memory_name, "i large err", sizeof(memory_name));
			return 0;
		}
		delay(200);
		while(!TF0);
		c =   get_total_mem_size();
		//i2c_WP = 1;
		i2c_type = c;
		//i2c_type = E24C08;//E24C08;
		if(0xFF == i2c_type)
		{
			memcpy(memory_name, "FF size err", sizeof(memory_name));
			dwMemorySize = 0;
			//wSectorSize = 0;
			dwSectorCount = 0;
			return fFlashOK;
		}
		else if(0xFE == i2c_type)
		{
			memcpy(memory_name, "FE size err", sizeof(memory_name));
			dwMemorySize = 0;
			//wSectorSize = 0;
			dwSectorCount = 0;
			return fFlashOK;
		}
		/*for(; 0x10 > i; i++)
		{
			//
			I2C_START;
			//delay(10);
			//while(!TF0);
			if(i2c_send_data(0xa0 | tmp))
			{
				memcpy(memory_name, "I2C FOUND ", sizeof(memory_name));
				//goto next;
			}
			else
				memcpy(memory_name, "I2C ERROR ", sizeof(memory_name));
			//delay(10);
			//while(!TF0);
			I2C_STOP;
			//delay(10);
			//while(!TF0);
			tmp++;
		}
		next:
		I2C_STOP;*/
		//
		if(i2c_type)
			dwMemorySize = i2c_size = ee_total_mem;
		else
			dwMemorySize = i2c_size = 16;
		//dwMemorySize = i2c_size = i2c_type * 128;// eeprom size in bytes
		i2c_page_size = ee_page_size;
		if(wSectorSize > i2c_size)
			dwSectorCount = 1;
		else
			dwSectorCount = i2c_size / wSectorSize;
		/*
		if(i2c_type > E24C16)
			i2c_large_size = 1;
		else
			i2c_large_size = 0;
		*/
//		memset(memory_name, 0x00, sizeof(memory_name));
		switch(i2c_type)
		{
			case E24C00:
			{
				memcpy(memory_name, "24C00      ", sizeof(memory_name));
				break;
			}
			case E24C01:
			{
				memcpy(memory_name, "24C01      ", sizeof(memory_name));
				break;
			}
			case E24C02:
			{
				memcpy(memory_name, "24C02      ", sizeof(memory_name));
				break;
			}
			case E24C04:
			{
				memcpy(memory_name, "24C04      ", sizeof(memory_name));
				break;
			}
			case E24C08:
			{
				memcpy(memory_name, "24C08      ", sizeof(memory_name));
				break;
			}
			case E24C16:
			{
				memcpy(memory_name, "24C16      ", sizeof(memory_name));
				break;
			}
			case E24C32:
			{
				memcpy(memory_name, "24C32      ", sizeof(memory_name));
				break;
			}
			case E24C64:
			{
				memcpy(memory_name, "24C64      ", sizeof(memory_name));
				break;
			}
			case E24C128:
			{
				memcpy(memory_name, "24C128     ", sizeof(memory_name));
				break;
			}
			case E24C256:
			{
				memcpy(memory_name, "24C256     ", sizeof(memory_name));
				break;
			}
			case E24C512:
			{
				memcpy(memory_name, "24C512     ", sizeof(memory_name));
				break;
			}
			case E24C1024:
			{
				memcpy(memory_name, "24C1024    ", sizeof(memory_name));
				break;
			}
			case E24C2048:
			{
				memcpy(memory_name, "24C2048    ", sizeof(memory_name));
				break;
			}
			default:
			{
				memcpy(memory_name, "error ic   ", sizeof(memory_name));
				break;
			}
		}
		fFlashOK = 1;
	}
	return fFlashOK;
}

void i2c_page_read(DWORD addr, BYTE * pdat)
{
	char control;
	char addr2;
	char addr1;
	char addr0;
	char xdata page_size;
	WORD xdata sector_size = wSectorSize;
	//
	if(0x80 != ee_address_size)
	{
		while(sector_size)
		{
			page_size = ee_page_size;
			
			addr2 = (BYTE)((addr & 0x00ff0000) >> 16);
			addr1 = (BYTE)((addr & 0x0000ff00) >> 8);
			addr0 = (BYTE)(addr & 0x000000ff);
			control = 0xa0;// | (i2c_slave_addr << 1);
			
			I2C_START;
			if(i2c_large_size)//if size > 24C16
			{
				control |= ((addr2 & 0x07) << 1) | I2C_WRITE;
				i2c_send_data(control);// | ((addr2 & 0x07) << 1) | I2C_WRITE);
				i2c_send_data(addr1);
			}
			else
			{
				control |= ((addr1 & 0x07) << 1) | I2C_WRITE;
				i2c_send_data(control);// | ((addr1 & 0x07) << 1) | I2C_WRITE);
			}
			i2c_send_data(addr0);
			I2C_START;
			i2c_send_data(control | I2C_READ);
			while(page_size--)
			{
				if(page_size)
				{
					I2C_RX;
				}
				else
				{
					I2C_RX_NAK;
				}
				*pdat++ = I2CRXD;
			}
			I2C_STOP;
			addr += ee_page_size;
			sector_size -= ee_page_size;
			if(addr >= dwMemorySize)
				return;
		}
	}
	else
	{
		while(sector_size)
		{
			page_size = ee_page_size;
			
			
			addr0 = ((BYTE)(addr & 0x000000ff)) << 1;
			
			I2C_START;

			i2c_send_data(addr0 | I2C_READ);
			while(page_size--)
			{
				if(page_size)
				{
					I2C_RX;
				}
				else
				{
					I2C_RX_NAK;
				}
				*pdat++ = I2CRXD;
			}
			I2C_STOP;
			addr += ee_page_size;
			sector_size -= ee_page_size;
			if(addr >= dwMemorySize)
				return;
		}
	}
}
//
void i2c_page_verify(DWORD addr, BYTE * pdat)
{
	char control;
	char addr2;
	char addr1;
	char addr0;
	char xdata page_size;
	DWORD verify_addr = 0;
	WORD xdata sector_size = wSectorSize;
	//
	if(0x80 != ee_address_size)
	{
		while(sector_size)
		{
			page_size = ee_page_size;
			
			addr2 = (BYTE)((addr & 0x00ff0000) >> 16);
			addr1 = (BYTE)((addr & 0x0000ff00) >> 8);
			addr0 = (BYTE)(addr & 0x000000ff);
			control = 0xa0;// | (i2c_slave_addr << 1);
			
			I2C_START;
			if(i2c_large_size)//if size > 24C16
			{
				control |= ((addr2 & 0x07) << 1) | I2C_WRITE;
				i2c_send_data(control);// | ((addr2 & 0x07) << 1) | I2C_WRITE);
				i2c_send_data(addr1);
			}
			else
			{
				control |= ((addr1 & 0x07) << 1) | I2C_WRITE;
				i2c_send_data(control);// | ((addr1 & 0x07) << 1) | I2C_WRITE);
			}
			i2c_send_data(addr0);
			I2C_START;
			i2c_send_data(control | I2C_READ);
			while(page_size--)
			{
				if(page_size)
				{
					I2C_RX;
				}
				else
				{
					I2C_RX_NAK;
				}
				//*pdat++ = I2CRXD;
				if(pdat[verify_addr] != I2CRXD)
				{
					memcpy(memory_name, "Verify  Err", sizeof(memory_name));
					verify_err = 1;
					scsi_ready = 0;
					verify_err_addr = addr + verify_addr;
				}
				verify_addr++;
			}
			I2C_STOP;
			addr += ee_page_size;
			sector_size -= ee_page_size;
			if(addr >= dwMemorySize)
				return;
		}
	}
	else
	{
		while(sector_size)
		{
			page_size = ee_page_size;
			
			
			addr0 = ((BYTE)(addr & 0x000000ff)) << 1;
			
			I2C_START;

			i2c_send_data(addr0 | I2C_READ);
			while(page_size--)
			{
				if(page_size)
				{
					I2C_RX;
				}
				else
				{
					I2C_RX_NAK;
				}
				//*pdat++ = I2CRXD;
				if(pdat[verify_addr] != I2CRXD)
				{
					memcpy(memory_name, "Verify  Err", sizeof(memory_name));
					verify_err = 1;
					scsi_ready = 0;
					verify_err_addr = addr + verify_addr;
				}
				verify_addr++;
			}
			I2C_STOP;
			addr += ee_page_size;
			sector_size -= ee_page_size;
			if(addr >= dwMemorySize)
				return;
		}
	}
}

/*
void i2c_page_write(DWORD addr, BYTE * pdat)
{
	return;
}
*/

void i2c_page_write(DWORD addr, BYTE * pdat)
{
	char control;
	char addr2;
	char addr1;
	char addr0;
	char xdata page_size;
	WORD xdata sector_size = wSectorSize;
	//
	//i2c_WP = 0;
	//
	if(0x80 != ee_address_size)
	{
		while(sector_size)
		{
			page_size = ee_page_size;
			
			addr2 = (BYTE)((addr & 0x00ff0000) >> 16);
			addr1 = (BYTE)((addr & 0x0000ff00) >> 8);
			addr0 = (BYTE)(addr & 0x000000ff);
			control = 0xa0 | (i2c_slave_addr << 1);
			
			I2C_START;
			if(i2c_large_size)//if size > 24C16
			{
				while(i2c_send_data(control | ((addr2 & 0x07) << 1) | I2C_WRITE))
				{
					I2C_STOP;
					I2C_START;
				}
				i2c_send_data(addr1);
			}
			else
			{
				while(i2c_send_data(control | ((addr1 & 0x07) << 1) | I2C_WRITE))
				{
					I2C_STOP;
					I2C_START;
				}
			}
			i2c_send_data(addr0);
			while(page_size--)
			{
				i2c_send_data(*pdat++);
			}
			I2C_STOP;
			addr += ee_page_size;
			sector_size -= ee_page_size;
			if(addr >= dwMemorySize)
				goto exit;
		}
	}
	else
	{
		while(sector_size)
		{
			page_size = ee_page_size;
			
			addr0 = ((BYTE)(addr & 0x000000ff)) << 1;
			
			I2C_START;
			while(i2c_send_data(addr0 | I2C_WRITE))
			{
				I2C_STOP;
				I2C_START;
			}
			while(page_size--)
			{
				i2c_send_data(*pdat++);
			}
			I2C_STOP;
			addr += ee_page_size;
			sector_size -= ee_page_size;
			if(addr >= dwMemorySize)
				goto exit;
		}
	}
	exit:
	I2C_START;
	while(i2c_send_data(0xA0 | I2C_WRITE))
	{
		I2C_STOP;
		I2C_START;
	}
	I2C_STOP;
	//i2c_WP = 1;
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//
//
//                      I2C MEMORY SIZE AUTODETECTION CODE
//
//
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//https://www.avrfreaks.net/s/topic/a5C3l000000UH0xEAG/t079197 original web page
//#define autodet

//#ifdef autodet

//void detect()
//{
//   c =   get_address_size();
//   serial_print_dec(c);
//   serial_printf( " byte address size (or error code)\r\n ");
//   c =   get_total_mem_size();
//   serial_print_dec(c);
//   serial_printf( " Total mem code (or error code)\r\n ");
//}

void write_i2c(BYTE slave_addr, BYTE dat)//addr xx
{
	//
	I2C_START;
	while(i2c_send_data(0xA0| I2C_WRITE))
	{
		I2C_STOP;
		I2C_START;
	}
	i2c_send_data(slave_addr);
	i2c_send_data(dat);
	I2C_STOP;
	I2C_START;
	while(i2c_send_data(0xA0 | I2C_WRITE))
	{
		I2C_STOP;
		I2C_START;
	}
	I2C_STOP;
	delay(50);
	while(!TF0);
}

void writeL_i2c(unsigned int slave_addr, BYTE dat)//addr 00 xx
{
	//
	I2C_START;
	while(i2c_send_data(0xA0 | I2C_WRITE))
	{
		I2C_STOP;
		I2C_START;
	}
	i2c_send_data((BYTE)(slave_addr >> 8));
	i2c_send_data((BYTE)(slave_addr & 0x00ff));
	i2c_send_data(dat);
	I2C_STOP;
	I2C_START;
	while(i2c_send_data(0xA0 | I2C_WRITE))
	{
		I2C_STOP;
		I2C_START;
	}
	I2C_STOP;
	delay(50);
	while(!TF0);
}

BYTE read_i2c(BYTE slave_addr)//addr xx
{
	//
	BYTE dat;
	delay(5);
	while(!TF0);
	I2C_START;
	while(i2c_send_data(0xA0 | I2C_WRITE))
	{
		I2C_STOP;
		I2C_START;
	}
	i2c_send_data(slave_addr);
		I2C_START;
		i2c_send_data(0xA0 | I2C_READ);
			I2C_RX_NAK;
			dat = I2CRXD;
	I2C_STOP;
	return dat;
}

BYTE readL_i2c(unsigned int slave_addr)//addr 00 xx
{
	//
	BYTE dat;
	delay(5);
	while(!TF0);
	I2C_START;
	while(i2c_send_data(0xA0 | I2C_WRITE))
	{
		I2C_STOP;
		I2C_START;
	}
	i2c_send_data((BYTE)(slave_addr >> 8));
	i2c_send_data((BYTE)(slave_addr & 0x00ff));
		I2C_START;
		i2c_send_data(0xA0 | I2C_READ);
			I2C_RX_NAK;
			dat = I2CRXD;
	I2C_STOP;
	return dat;
}

unsigned char ping_i2c(unsigned char slave_addr)
{
	unsigned char x;
	//start();		// send start bit
	I2C_START;
	//write_i2c(slave_addr);	// test this address
	//i2c_send_data(slave_addr);
	// this line needs to be converted to TWI for AVR chips
	//if (!sspcon2.ACKSTAT)	// did we get an ACK?
	if(!i2c_send_data(slave_addr))
		x = 1;
	else
		x = 0;
	//stop();		// send stop bit
	I2C_STOP;
	return x; 
}

unsigned char test_i2c(unsigned char address)
{
	unsigned char ee_save, ee_test, c, ee_start;
// routine that tests writing at a address then changes it back
	ee_start = read_i2c(0x00);
	ee_save = read_i2c(address);
	if(ee_save!=0x55) {
		write_i2c(address,0x55);
		ee_test = 0x55;
	} else {
		write_i2c(address,0xCC);
		ee_test = 0xCC;
	}
	c = read_i2c(address);
	if(c == read_i2c(0x00))
	{
		// address is not working | wrapped
		write_i2c(address,ee_save);	// restore old data
		return 0;
	}
	if(c == ee_test) {
		// address is working
		write_i2c(address,ee_save);	// restore old data
		return 1;
	} else {
		if(c==ee_save)
			return 0;
		else {
			write_i2c(address,ee_save);	// try to restore old data
			return 0xff;	// strange results
		}
	}
}

unsigned char testL_i2c(unsigned int address)
{
	unsigned char ee_save, ee_test, c, ee_start;
// routine that tests writing at a address then changes it back
	ee_start = readL_i2c(0x0000);
	ee_save = readL_i2c(address);
//	if((ee_start != 0xff) && (ee_start == ee_save) && address)
//		return 0;
	if(ee_save!=0x55) {
		writeL_i2c(address,0x55);
		ee_test = 0x55;
	} else {
		writeL_i2c(address,0xCC);
		ee_test = 0xCC;
	}
	c = readL_i2c(address);
	if(c == readL_i2c(0x0000))
	{
		// address is not working | wrapped
			writeL_i2c(address,ee_save);	// restore old data
			return 0;
	}
	if(c == ee_test)
		{
			// address is working
			writeL_i2c(address,ee_save);	// restore old data
			return 1;
		} 
		else 
		{
			if(c==ee_save)
				return 0;
			else {
				writeL_i2c(address,ee_save);	// try to restore old data
				return 0xff;	// strange results
		}
	}
}

unsigned char get_address_size()
{
	unsigned char ee_save;
	WORD i = 0, j = 0;
	BYTE xdata buf[256];
	BYTE xdata buf2[256];
	memset(buf, 0xFF, 256);
	memset(buf2, 0xFF, 256);
	
	// test 2400/2401 128x1 addressing
	if(ping_i2c(0x00))
	{ 
		do
		{
			
			if(ping_i2c(i))
				j++;
			i=i+2;
		}while(i < 0xFF);
		ee_address_size = j;
		return ee_address_size;
	}
	else
	{
	
		 ee_save = read_i2c(0);     // Read memory address 00
		
		//********************************************************************************************************
		I2C_START;
		while(i2c_send_data(0xa0 | I2C_WRITE))
		{
			I2C_STOP;
			I2C_START;
		}
		i2c_send_data(0x00); // Read 16-bit memory / write 8-bit memory with NACK
		i2c_send_data(0x01);
		I2C_STOP;
		I2C_START;
			if(i2c_send_data(0xa0 | I2C_WRITE))
			{
				I2C_STOP;
				ee_address_size = 1;
				write_i2c(0, ee_save);
			}
			else
			{
				I2C_STOP;
				ee_address_size = 2;
			}
			//return ee_address_size;
			//********************************************************************************************************
	}
	// detect page size ee_page_size = 
	//
	// backup read
	I2C_START;
	while(i2c_send_data(0xA0 | I2C_WRITE))
	{
		I2C_STOP;
		I2C_START;
	}
	i2c_send_data(0x00);
	if(2 == ee_address_size)
		i2c_send_data(0x00);
	I2C_STOP;
	//
	I2C_START;
	i2c_send_data(0xA0 | I2C_READ);
	for(i = 0; i < 256; i++)
	{
		if(i < 255)
		{
			I2C_RX;
		}
		else
		{
			I2C_RX_NAK;
		}
		buf[i] = I2CRXD;
	}
	I2C_STOP;
	//------------------------------------------------------------------ write 5AA5
	I2C_START;
	while(i2c_send_data(0xA0 | I2C_WRITE))
	{
		I2C_STOP;
		I2C_START;
	}
	i2c_send_data(0x00);
	if(2 == ee_address_size)
		i2c_send_data(0x00);
	//
	for(i = 0; i < 256; i++)
	{
//		if(i < 128)
//			i2c_send_data(0x5A);
//		else
			i2c_send_data(0xA5);
	}
	I2C_STOP;
	//------------------------------------------------------------------ test read
	I2C_START;
	while(i2c_send_data(0xA0 | I2C_WRITE))
	{
		I2C_STOP;
		I2C_START;
	}
	i2c_send_data(0x00);
	if(2 == ee_address_size)
		i2c_send_data(0x00);
	I2C_STOP;
	//
	I2C_START;
	i2c_send_data(0xA0 | I2C_READ);
	for(i = 0; i < 256; i++)
	{
		if(i < 255)
		{
			I2C_RX;
		}
		else
		{
			I2C_RX_NAK;
		}
		buf2[i] = I2CRXD;
	}
	I2C_STOP;
	//
	if(memcmp(buf2, buf2 + 1, 1))
		ee_page_size = 1;
	else if(memcmp(buf2, buf2 + 2, 2))
		ee_page_size = 2;
	else if(memcmp(buf2, buf2 + 4, 4))
		ee_page_size = 4;
	else if(memcmp(buf2, buf2 + 8, 8))
		ee_page_size = 8;
	else if(memcmp(buf2, buf2 + 16, 16))
		ee_page_size = 16;
	else if(memcmp(buf2, buf2 + 32, 32))
		ee_page_size = 32;
	else if(memcmp(buf2, buf2 + 64, 64))
		ee_page_size = 64;
	else if(memcmp(buf2, buf2 + 128, 128))
		ee_page_size = 128;
	else
		ee_page_size = 256;
	//
	//------------------------------------------------------------------ test 24c02
	if(1 == ee_address_size)
	{
		// 2nd read
		if(ping_i2c(0xA2))
		{
			I2C_START;
			while(i2c_send_data(0xA2 | I2C_WRITE))
			{
				I2C_STOP;
				I2C_START;
			}
			i2c_send_data(0x00);
			I2C_STOP;
			//
			I2C_START;
			i2c_send_data(0xA2 | I2C_READ);
			for(i = 0; i < 256; i++)
			{
				if(i < 255)
				{
					I2C_RX;
				}
				else
				{
					I2C_RX_NAK;
				}
				buf2[ee_page_size + i] = I2CRXD;
			}
			I2C_STOP;
			//
			if(!memcmp(buf2, buf2+ee_page_size, ee_page_size))
			{
				ee_segment_size = 256;
        ee_total_mem = 256;
			}
//			if(!memcmp(buf2, buf2+128, ee_page_size))
//			{
//				ee_segment_size = 128;
//        ee_total_mem = 128;
//			}
		}
	}
	//------------------------------------------------------------------ backup write
	I2C_START;
	while(i2c_send_data(0xA0 | I2C_WRITE))
	{
		I2C_STOP;
		I2C_START;
	}
	i2c_send_data(0x00);
	if(2 == ee_address_size)
		i2c_send_data(0x00);
	for(i = 0; i < ee_page_size; i++)
	{
		i2c_send_data(buf[i]);
	}
	I2C_STOP;
	//
	return ee_address_size;
}
//

unsigned int get_total_mem_size()
{
	unsigned char c,i;//ee_test_address, ee_save, ee_test, c, i;
	
	ping_i2c(ee_address_size);
	if(0x80 == ee_address_size)// 128 addresses means AT24C01
	{
		ee_page_size = 1;
		ee_segment_size = 128;
		ee_total_mem = 128;
		return 1;
	}
   // first determine 8bit address total memsize
   else if(ee_address_size == 1) 
	 {
			//
      if(test_i2c(0x0f) != 1)      // largest mem location for24LC00
         return 0xfe;
      if(test_i2c(0x10) != 1) {   // fail 16 bytes therefore 24LC00
//         ee_page_size = 1;
         ee_segment_size = 16;
         ee_total_mem = 16;
         return 0;
      }
      if(test_i2c(0x80) != 1) {   // fail 129 bytes therefore 24LC01B
//				a24c01:
//         ee_page_size = 8;
         ee_segment_size = 128;
         ee_total_mem = 128;
         return 1;
      }
			//
			if(256 == ee_total_mem)
				goto a24c02;
			//
      c = 0;
      for (i=0xA0; i < 0xB0; i=i+2) {
         if(ping_i2c(i))
            c++;
      }
      if(c==1) {               // only one address means 24LC02B
				a24c02:
//         ee_page_size = 8;
         ee_segment_size = 256;
         ee_total_mem = 256;
         return 2;
      }
      if(c==2) {               // 2 addresses means 24LC04B
//         ee_page_size = 16;
         ee_segment_size = 256;
         ee_total_mem = 512;
         return 4;
      }
      if(c==4) {               // 4 addresses means 24LC08B
//         ee_page_size = 16;
         ee_segment_size = 256;
         ee_total_mem = 1024;
         return 8;
      }
      if(c==8) {               // 8 addresses means 24LC16B
//         ee_page_size = 16;
         ee_segment_size = 256;
         ee_total_mem = 2048;
         return 16;
      }
      // failure for single address
      ee_page_size = 0;
      ee_segment_size = 0;
      ee_total_mem = 0;
      return 0xff;
   } 
	 else 
		 {
			//
			if(ee_page_size >= 128)
				goto ee_large;
			//
   // now testing 16 bit addresses
      if(testL_i2c(0xfff) != 1)      // largest mem location for 24LC32B
         return 0;
      if(testL_i2c(0x1000) != 1) {   // fail 4K+1 bytes therefore 24LC32B
//         ee_page_size = 32;
         ee_segment_size = 4096;
         ee_total_mem = 4096;
         return 32;
      }
      if(testL_i2c(0x2000) != 1) {   // fail 8K+1 bytes therefore 24LC65
         //ee_page_size = 64;
//				ee_page_size = 32;
         ee_segment_size = 8192;
         ee_total_mem = 8192;
         //return 65;
				return 64;
      }
      if(testL_i2c(0x4000) != 1) {   // fail 16K+1 bytes therefore 24LC128
         //ee_page_size = 64;
//				ee_page_size = 32;
         ee_segment_size = 16384;
         ee_total_mem = 16384;
         return 128;
      }
      if(testL_i2c(0x8000) != 1) {   // fail 32K+1 bytes therefore 24LC256
         //ee_page_size = 64;
//				ee_page_size = 32;
         ee_segment_size = 32768;
         ee_total_mem = 32768;
         return 256;
      }
			ee_large:
      c = 0;
      for (i=0xA8; i < 0xB0; i=i+2) {
         if(ping_i2c(i))
            c++;
      }
      if(c == 0) {
         //ee_page_size = 64;         // only one address might mean 24LC512
//				ee_page_size = 32;
         ee_segment_size = 65536;
         ee_total_mem = 65536;
         return 512;
      }
      if(c == 1) {
         //ee_page_size = 128;         // two addresses should mean 24LC1025
//				ee_page_size = 32;
         ee_segment_size = 65536;
         ee_total_mem = 131072;
         return 1024;
      }
   // failure for 16 bit addresses
   ee_page_size = 0;
   ee_segment_size = 0;
   ee_total_mem = 0;
   return 0xff;
	}
}
	 
//#endif
