#ifndef __RFE_PARAMS_H__
#define __RFE_PARAMS_H__

#include "stc.h"
#include "main.h"

// Setup structure / any set may be loaded from eeprom on start
typedef struct{
	char speed;					// interface speed
	char voltage;				// autodetect, fixed 3.3v, fixed 1.8v
	char autoverify;		// autoverify on/off
	char ocp_level;			// ocp current level
	char ocp_speed;			// ocp reaction speed
	char isp_mode;			// isp high-current mode on/off
} setup_t;	// constant/init values in eeprom
extern setup_t code rfe_setup;

// data params after ic init
typedef struct{
	struct{
		char speed;					// interface speed
		char voltage;				// autodetect, fixed 3.3v, fixed 1.8v
		char autoverify;		// autoverify on/off
		char ocp_level;			// ocp current level
		char ocp_speed;			// ocp reaction speed
		char isp_mode;			// isp high-current mode on/off
	} setup_params;
	struct{
		char mode;		// spi, i2c, spi-ee, emmc, etc..
		char ic_detect_status;	// status of ic detection tests
		char cpu_speed;			// main cpu frequency
	} device_params;
	struct{
		char ic_name[11];		// ic name
		char ic_id[3];			// ic id
		char ic_type[3];		// type of memory ic
		char ic_size;				// ic memory size
		char ic_voltage;		// ic voltage
	} ic_params;
}active_t;
extern active_t xdata rfe_active;

extern bool manual_setup;

extern void xdata * copy_labels(char xdata * buf_addr, char code * code_addr);
extern void xdata * copy_params(char xdata * buf_addr, char code * code_addr, char spacer);// SPACER == 0 --> NEW LINE CRLF

extern char code text_label_main;
extern char code text_label_warning;
extern char code text_label_ic_size;

extern char code text_label_i2c_mem_layout;

extern char code Reg1Set1;
extern char code Reg1Set2;
extern char code Reg2Set1;
extern char code Reg3Set1;

extern char code label_pin;
extern char code label_nc_text;
extern char code label_shorted;

extern char code label_auto;
extern char code label_manual;

extern char code label_mhz;
extern char code label_mv;

extern char code label_v18;
extern char code label_v33;

extern char code label_autoneg;
extern char code label_id_error;
extern char code label_ver_error;

extern char code label_at;
extern char code label_hex_0x;

extern char code label_magic;
extern char code label_ping;
extern char code label_sfdp;
extern char code label_id;
		
extern char code label_qpi;
extern char code label_qspi;
extern char code label_dspi;

extern char code label_spi;
extern char code label_i2c;
extern char code label_twi;
extern char code label_mmc;
extern char code label_flash;
extern char code label_eeprom;
extern char code label_emmc;
extern char code label_spinand;

// insert code here

extern char code label_overload;
extern char code label_chip180;
extern char code label_bus_error;

//extern char code label_null;

extern char code label_pinout;

extern char code label_ok;
extern char code label_nc;
extern char code label_error;
extern char code label_vcc;
extern char code label_gnd;

////////////////////////////////////// old style
//extern char code label_spi_cs;
//extern char code label_spi_mosi;
//extern char code label_spi_wp;
//extern char code label_spi_gnd;
//extern char code label_spi_miso;
//extern char code label_spi_sclk;
//extern char code label_spi_hold;
//extern char code label_spi_vcc;

////////////////////////////////////// 8x6 array
extern char code label_spi_pins_arr;

extern char code label_i2c_pins_arr;

extern char code label_io_test1;
extern char code label_io_test2;

extern char code label_about;
extern char code label_host;
extern char code label_windows;
extern char code label_linux;
extern char code label_apple;
extern char code label_build;
extern char code label_version;
extern char code label_serial_number;
extern char code label_hello;
extern char code label_copyright;

extern char code label_build2;

#endif //__RFE_PARAMS_H__