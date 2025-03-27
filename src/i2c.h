#ifndef __I2C_H__
#define __I2C_H__

#define I2C_ENABLE				0x80
#define I2C_MASTER_MODE		0x40

#define I2C_PORT_1		0
#define I2C_PORT_2		1
#define I2C_PORT_3		3
#define I2C_PORT_7		2

#define I2C_100K			58
#define I2C_200K			28
#define I2C_300K			18
#define I2C_400K			13
#define I2C_500K			10
#define I2C_600K			8
#define I2C_750K			6
#define I2C_1000K			4
#define I2C_1500K			2
#define I2C_2000K			1
/*
#define E24C01				128
#define E24C02				256
#define E24C04				512
#define E24C08				1024
#define E24C16				2048
#define E24C32				4096
#define E24C64				8192
#define E24C128				16384
#define E24C256				32768
#define E24C512				65536
#define E24C1024			131072
#define E24C2048			262144*/

/*
#define I2C_READ				1
#define I2C_WRITE				0
#define I2C_BUSY		(0x80 & I2CMSST)
#define I2C_RDY			(0x40 & I2CMSST)
#define I2C_RDY_CLR	(I2CMSST &= ~0x40)
#define I2C_ACK_RX	(0x02 & I2CMSST)
#define I2C_ACK_TX	(I2CMSST &= ~0x01)
#define I2C_NACK_TX	(I2CMSST |= 0x01)

#define I2C_START		do{I2CMSCR = 0x01; while(!I2C_RDY); I2C_RDY_CLR;}while(0)
#define I2C_STOP		do{I2CMSCR = 0x06; while(!I2C_RDY); I2C_RDY_CLR;}while(0)
//#define I2C_RESTART	(I2C_STOP; I2C_START;)
#define I2C_TX			do{I2CMSCR = 10; while(!I2C_RDY); I2C_RDY_CLR;}while(0)
#define I2C_RX			do{I2CMSCR = 11; while(!I2C_RDY); I2C_RDY_CLR;}while(0)
#define I2C_RX_NAK	do{I2CMSCR = 12; while(!I2C_RDY); I2C_RDY_CLR;}while(0)
*/

#define memory_init         i2c_init
#define memory_check        i2c_check
#define memory_readsector   i2c_page_read
#define memory_writesector  i2c_page_write


extern BYTE ee_address_size;
extern BYTE ee_page_size;
extern DWORD ee_segment_size;
extern DWORD ee_total_mem;


BOOL i2c_init(char port, char i2c_speed, char init_level);
void i2c_disable();
BOOL i2c_check();

BOOL i2c_send_data(BYTE dat);
void i2c_page_read(DWORD addr, BYTE * pdat);
void i2c_page_verify(DWORD addr, BYTE * pdat);
void i2c_page_write(DWORD addr, BYTE * pdat);

BYTE get_address_size();
WORD get_total_mem_size();

#endif