#ifndef __SPI_H__
#define __SPI_H__

#define spi_select          SPI_SS = 0
#define spi_deselect        SPI_SS = 1


#define SPI_WREN            0x06
#define SPI_WRDI            0x04
#define SPI_RDSR1           0x05
#define SPI_RDSR2           0x35
#define SPI_RDSR3           0x15
#define SPI_RDSR4           0x95
#define SPI_WRSR1           0x01
#define SPI_WRSR2           0x31
#define SPI_WRSR3           0x11
#define SPI_READ            0x03
#define SPI_READ32          0x13
#define SPI_FASTREAD        0x0B
#define SPI_PAGEPROG        0x02
#define SPI_RDCR            0xA1
#define SPI_WRCR            0xF1
//#define SPI_SECTORER        0x20
#define SPI_BLOCKER32       0x52
#define SPI_BLOCKER64       0xD8
//#define SPI_CHIPER          0xC7
#define SPI_RDID            0xAB
#define SPI_REMS	          0x90
#define SPI_JDID           0x9F
#define SPI_UUID            0x4B // 64-bit unique id after 4 dummy bytes
#define SPI_SFDP            0x5A // 256-byte flash info page

#define SPI_QPI_EN           0x38// QPI enable (Winbond)
#define SPI_QPI_EX           0xFF// QPI disable (Winbond)
#define SPI_QPI_EN_MX        0x35// QPI enable (MXIC)
#define SPI_QPI_EX_MX        0xF5// QPI disable (MXIC)

#define SPI_WREAR						0xC5// write extended addr register
#define SPI_RDEAR						0xC8// read extended addr register
#define SPI_EN4B						0xB7// enter 4-byte addr mode
#define SPI_EX4B						0xE9// exit 4-byte addr mode

										// need WEL=1
#define SPI_GBU							0x98//Global Block/Sector Unlock (98h)
#define SPI_GBL							0x7E//Global Block/Sector Lock (7Eh)
#define SPI_IBR							0x3D//Read Block/Sector Lock (3Dh)
#define SPI_IBU							0x39//Individual Block/Sector Unlock (39h)
#define SPI_IBL							0x36//Individual Block/Sector Lock (36h)

#define SST_AAI_EN					0xAD



#define WINBOND			0xEF
#define MXIC				0xC2
#define EON					0x1C
#define SST					0xBF
#define ISSI				0x9D
#define GD					0xC8	
#define MD					0x51// GD No HOLD Pin MD25D80 51:40:14
#define ST					0x20// and XMIC
#define XMIC				0x20
#define PMC					0x7F
//#define AMIC				0x7F
//#define PMC					0x7F9D
//#define AMIC				0x7F37
#define AMIC				0x37
#define ATMEL				0x1F
#define SPANSION		0x01
#define PUYA				0x85// SY25Q16/P25D16 85:60:15/PY25Q64 85:20:17
#define BOYA				0x68// No HOLD Pin BY25D80 68:40:14
#define INTEL				0x89
#define INTEL2			0xD8// MK25Q32 D8:40:16
#define ESI					0x4A
#define NANSI				0xD5
#define NTRCS				0xD5
#define ZBIT				0x5E
#define XTX					0x0B



extern char xdata SPI_SECTORER;
extern char xdata SPI_CHIPER;
void spi_detect();


#define SPI_SS_DISABLE		0x80
#define SPI_ENABLE				0x40
#define SPI_LSB_ORDER			0x20
#define SPI_MASTER_MODE		0x10
#define SPI_SCLK_POL_HI		0x08
#define SPI_SCLK_PHASE		0x04

#define SPIF    0x80
#define WCOL    0x40

#define SPI_PORT_1				0
#define SPI_PORT_2				1
#define SPI_PORT_3				3
#define SPI_PORT_4				2

#define SPI_DIV2	0
#define SPI_DIV4	1
#define SPI_DIV8	2
#define SPI_DIV16	3

#define SPI_CS			P54
sbit SPI_SS     =   P5^4;
sbit SPI_HOLD	 	=		P1^7;
sbit SPI_WP			=		P1^6;

extern BOOL reg_wr_test;
extern BOOL reg_wr_flag;
extern BOOL QPI_PRESENT;
extern BOOL vcc_sfdp;
extern BOOL spi_4byte;

BOOL spi_init(char port, char speed, char init_level);
char spi_check(char speed, char init_level);
void spi_disable();



BYTE spi_read_reg(BYTE reg);
//BYTE spi_write_reg(BYTE reg, BYTE dat1, BYTE dat2, BYTE dat3);
BYTE spi_write_reg(BYTE reg, BYTE dat1, BYTE dat2);

// len: 0/1
#define CONT	(char)0x00// byte array
#define END		(char)0x01// one/last byte
BYTE spi_write_custom_byte(char dat, bool len) small;

extern BYTE spi_send(BYTE dat) small;//asm
extern BOOL spi_isbusy() small;//asm
extern BOOL spi_parity_check(BYTE dat) small;//asm
extern BYTE spi_id_remap(BYTE target_id, BYTE code * remap_table);//asm
extern void set_ext_addr(char cmd, char addr);//asm

void spi_cmd(BYTE cmd, DWORD addr) small;

void spi_write_enable() small;

void spi_readsector(DWORD addr, BYTE xdata *pdat);

void sst_aai_enable();


extern WORD spi_ver_err;

//extern BYTE xdata spi_JDID_id[14];
extern BYTE xdata * spi_JDID_id;

//extern BYTE xdata sfdps[256];
extern BOOL sfdp_present;
extern BOOL spi_hold_swap;
extern BYTE xdata spi_regs[3];
//extern DWORD data mem_addr;


extern BYTE sfdp_read(BYTE * buf, WORD len);

#endif