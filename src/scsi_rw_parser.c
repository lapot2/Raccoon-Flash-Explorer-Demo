#include "main.h"
#include "stc.h"
#include "usb.h"
#include "usb_desc.h"
#include "scsi.h"
#include "util.h"
#include "memory.h"
#include "spi.h"
#include "i2c.h"
#include "pin_checker.h"
#include "hw_monitor.h"
#include "scsi_rw_parser.h"
#include "sfdp.h"

extern char code param_1;
extern char code param_2;

//
////////////////////////////////////////////////////////************************ READ SECTION ***************************
/*void read_fakefat()
{
	//
}*/
//
BOOL verify_enabled = 1;
extern BYTE idata spi_speed;
extern void spi_set_speed(BYTE speed) small;//0/1/2/3
//
void read_mem(DWORD addr, int nSize) small
{
	BYTE cnt;
	if (nSize > residue) nSize = residue;
	residue -= nSize;
	//
	if(!spi && verify_err) return;
	
	usb_write_reg(INDEX, 1);
	usb_write_reg(INCSR2, usb_read_reg(INCSR2) | INAUTOSET | INENDMA | INFCDT);
	usb_write_reg(INMAXP, EP1IN_SIZE);
	
	spi_cmd(SPI_READ, addr);

	while(nSize)
	{
			do
			{
				while (USBADR & UBUSY);
				USBADR = INCSR1 | UREAD;
				while (USBADR & UBUSY);
			} while (USBDAT & INIPRDY);
			
			USBADR = FIFO1 | UREAD;
			//
			cnt = EP1IN_SIZE;
			while(cnt--)
			{
				while(USBADR & UBUSY);
				USBDAT = spi_send(0);
			}
			nSize -= EP1IN_SIZE;
	}
	SPI_SS = 1;
}
//
//String Table
// IO level test / Not used
typedef struct{
	char header1[15], Pad_h1[2];
	char header2[18], Pad_h2[2];
	char header3[21], Pad_h3[2];
	char pin1[4], Spacer1[2], hi1[7], Pad1_1[2], lo1[6], Pad1_2[2];
	char pin2[4], Spacer2[2], hi2[7], Pad2_1[2], lo2[6], Pad2_2[2];
	char pin3[4], Spacer3[2], hi3[7], Pad3_1[2], lo3[6], Pad3_2[2];
	char pin4[4], Spacer4[2], hi4[7], Pad4_1[2], lo4[6], Pad4_2[2];
	char pin5[4], Spacer5[2], hi5[7], Pad5_1[2], lo5[6], Pad5_2[2];
	char pin6[4], Spacer6[2], hi6[7], Pad6_1[2], lo6[6], Pad6_2[2];
	char pin7[4], Spacer7[2], hi7[7], Pad7_1[2], lo7[6], Pad7_2[2];
	char pin8[4], Spacer8[2], hi8[7], Pad1_8[2], lo8[6], Pad8_2[2];
	char Pad_end[2];
}IO_TEST;
// IC Pinout image
typedef struct{
	char line1[22]; char Pad1[2];
	char line2[22]; char Pad2[2];
	char line3_1[5]; char pin1_param[5]; char line3_2[3]; char pin8_param[5]; char line3_3[4]; 
	char Pad3[2];
	
	char line4[22]; char Pad4[2];
	char line5_1[5]; char pin2_param[5]; char line5_2[3]; char pin7_param[5]; char line5_3[4]; 
	char Pad5[2];
	
	char line6[22]; char Pad[2];
	char line7_1[5]; char pin3_param[5]; char line7_2[3]; char pin6_param[5]; char line7_3[4]; 
	char Pad7[2];
	
	char line8[22]; char Pad8[2];
	char line9_1[5]; char pin4_param[5]; char line9_2[3]; char pin5_param[5]; char line9_3[4]; 
	char Pad9[2];
	
	char line10[22]; char Pad10[2]; char Pad11[2];
}IC_PINOUT_IMG;
// I2C Memory map
typedef struct{
	char banks_label[7];
	char banks_param[8];
	int spacer1;
	char bytes_label[7];
	char bytes_param[8];
	int spacer2;
	char pages_label[7];
	char pages_param[8];
	long spacer3;
}I2C_MEMORY_MAP;
// Spi registers map
typedef struct{
	char reg_label[9];
	char reg_param[2];
	int spacer1;
	
	char bit0_label[8];
	char bit0_param[1];
	int spacer2;
	char bit1_label[8];
	char bit1_param[1];
	int spacer3;
	char bit2_label[8];
	char bit2_param[1];
	int spacer4;
	char bit3_label[8];
	char bit3_param[1];
	int spacer5;
	
	char bit4_label[8];
	char bit4_param[1];
	int spacer6;
	char bit5_label[8];
	char bit5_param[1];
	int spacer7;
	char bit6_label[8];
	char bit6_param[1];
	int spacer8;
	char bit7_label[8];
	char bit7_param[1];
	long spacer9;
}SPI_REGISTER_MAP;

// IC Info labels (for all modes)
typedef struct{
	char mode_label[9];		//AUTO 1.8v Spi Flash 
	char mode_param[39];
	int spacer1;
	
	char ic_type_label[9];	//MX25U512   
	char ic_type_param[23];
	int spacer2;
	
	char ic_id_label[9];	//C2253A
	char ic_id_param[23];
	int spacer3;
	
	char ic_misc_label[9];	//QPI, SFDP, etc...
	char ic_misc_param[23];
	int spacer4;
	char ic_info_spacer1[2];//spacer
	//
	char ic_size_label[9];	//IC Size: by ID
	char ic_size_param[23];
	int spacer5;
	
	char mb_label[7];		//MB-->: 64 
	char mb_param[17];
	int spacer6;
	
	char kib_label[7];		//KiB->: 65536
	char kib_param[17];
	int spacer7;
	
	char bytes_label[7];	//Bytes: 67108864
	char bytes_param[17];
	int spacer8;
	
	char hex_label[7];		//Hex->: 0x4000000
	char hex_param[17];
	long spacer9;
//spacer2
}IC_INFO_HEADER;
//xdata IC_INFO_HEADER xdata * icinfo = scsibuf;// +4b code -2b ram
//data SPI_REGISTER_MAP xdata * spi_register = nullptr;
//
static void reg_bit_map(SPI_REGISTER_MAP * spi_register, char dat)
{
	data char i = 0;
	xdata char reg_bits[8];
	
	for(; i < 8; i++)
	{
		reg_bits[i] = ((dat >> i) & 1) + 0x30;
	}
	*spi_register->bit0_param = reg_bits[0];
	*spi_register->bit1_param = reg_bits[1];
	*spi_register->bit2_param = reg_bits[2];
	*spi_register->bit3_param = reg_bits[3];
	*spi_register->bit4_param = reg_bits[4];
	*spi_register->bit5_param = reg_bits[5];
	*spi_register->bit6_param = reg_bits[6];
	*spi_register->bit7_param = reg_bits[7];
}
//
void read_file1()
{
	///// Pointers void: 00 - data, 01 - xdata, fe - pdata, ff - code
	////////////////////
	xdata IC_INFO_HEADER xdata * ic_info = scsibuf;//	static ?
	xdata SPI_REGISTER_MAP xdata * spi_register = nullptr;
	xdata I2C_MEMORY_MAP xdata * i2c_mem_map = nullptr;
	xdata IC_PINOUT_IMG xdata * ic_pinout = nullptr;//	static ?
	
	data char xdata * ptr1 = scsibuf;//	static ?
	data char xdata * ptr2 = nullptr;
	pdata char code * ptr3[5] = {nullptr};
	data char xdata * ptr4 = nullptr;
	pdata char xdata * ptr5[8] = {nullptr};//	static data?
	data char code * ptr6 = nullptr;
//	data char code * ptr7 = nullptr;
	struct error * err_ptr = errorlog;
	data char i, j;
	
	error_clear();
	memset_xram(scsibuf, 0x20, 4096);

	ptr1 = copy_labels(ptr1, &text_label_main);
	if(fFlashOK) ptr1 = copy_labels(ptr1, &text_label_ic_size);
	
	ptr3[0] = manual_setup ? &label_manual : &label_auto;
	ptr3[1] = V18 ? &label_v18 : &label_v33;
	
	///////////////////////////////////////////////////// IC type selector
	{
		switch(((char)spi << 1) | i2c)//mode
		{
			case 0x02:// spi mode
			{
				//copying labels
				// Spi Status registers
//				if(!verify_err)// ugly
				{
					spi_register = (SPI_REGISTER_MAP *)ptr1;
					if(0xc2 == spi_JDID_id[0]) ptr1 = copy_labels(ptr1, &Reg1Set2);
					else ptr1 = copy_labels(ptr1, &Reg1Set1);
					// reg1 code here
					hex2str(&spi_register->reg_param, &spi_regs[0], 1);
					reg_bit_map(spi_register, spi_regs[0]);
					// reg1 code end
					
					if(spi_regs[1] != 0xff) 
					{
						spi_register = (SPI_REGISTER_MAP *)ptr1;
						ptr1 = copy_labels(ptr1, &Reg2Set1);
						//reg2 code here
						hex2str(&spi_register->reg_param, &spi_regs[1], 1);
						reg_bit_map(spi_register, spi_regs[1]);
						//reg2 code end
					}
					
					if(spi_regs[2] != 0xff) 
					{
						spi_register = (SPI_REGISTER_MAP *)ptr1;
						ptr1 = copy_labels(ptr1, &Reg3Set1);
						//reg3 code here
						hex2str(&spi_register->reg_param, &spi_regs[2], 1);
						reg_bit_map(spi_register, spi_regs[2]);
						//reg3 code end
					}
				}
				// Spi Status registers END
				//
				//copying params
				ptr3[2] = &label_spi;
				ptr3[3] = &label_flash;
				// Spi JEDEC ID
				hex2str(&ic_info->ic_id_param, spi_JDID_id, 3);
				// Misc params - QPI, Quad, Dual, SFDP, etc...
				ptr4 = &ic_info->ic_misc_param;
				if(QPI_PRESENT)  {ptr4 = copy_params(ptr4, &label_qpi, ' ');}
				if(sfdp_present) 
				{
					ptr4 = copy_params(ptr4, &label_sfdp, ' ');
					ptr4 = copy_params(&ic_info->ic_size_param, &label_sfdp, ' ');
// debug sfdp
#ifdef SFDP_TEST
//					hex2str(ptr4, &sfdps.raw[sfdps.TBL.mem_size_ptr+4],4);
					hex2str(ptr4, (BYTE *)&sfdp_mem_size,4);//sfdp_mem_size
#endif//SFDP_TEST
				}
				else copy_params(&ic_info->ic_size_param, &label_id, ' ');
				//
				//temp old-style copy
				memcpy(&ic_info->ic_type_param, memory_name, 11);
				//
				//test
				// Pin labels in ptr array
				// 8x6 array
				ptr6 = &label_spi_pins_arr;
				break;
			}
			case 0x01:// i2c mode
			{
				//copying labels
				i2c_mem_map = (I2C_MEMORY_MAP *)ptr1;
				ptr1 = copy_labels(ptr1, &text_label_i2c_mem_layout);
				// I2C Memory map
				uint2str(&i2c_mem_map->banks_param, ee_total_mem / ee_segment_size);
				uint2str(&i2c_mem_map->bytes_param, ee_page_size);
				uint2str(&i2c_mem_map->pages_param, ee_total_mem / ee_page_size);
				//
				//copying params
				ptr3[2] = &label_i2c;
				ptr3[3] = &label_eeprom;
				//
				//temp old-style copy
				memcpy(&ic_info->ic_type_param, memory_name, 11);
				
				//test
				// Pin labels in ptr array
				// 8x6 array
				ptr6 = &label_i2c_pins_arr;
				break;
			}
			default:	// ic not detected
			{
				if(!manual_setup) ptr3[1] = nullptr;
				break;
			}
		}
	}
	//
	///////////////////////////////////////////////////// IC MODE parameters frame
	{
		ptr2 = &ic_info->mode_param;
		for(i = 0, j = ' '; i < 4; i++)// -1 bytes in data/ +1-10 in xdata
		{
			ptr2 = copy_params(ptr2, ptr3[i], j);
		}
	}
	//
	///////////////////////////////////////////////////// IC Warning frame / Error collector
	{
		/////////////////////////////////////////////////// Error collect
		// test part
		j = ' ';
			// Autoneg by:
		if(fFlashOK && (3 > (SPCTL & 0x03)) && !(spi_ver_err & 0xf000) && ((pin_test & 0x00FF) == 0xFF) && pin_test && !i2c)
		{
//			if(!spi_ver_err) ptr4 = (char xdata *)&label_id_error;
			if(!spi_ver_err && !verify_err) ptr4 = (char xdata *)&label_id_error;// variant 2
			else ptr4 = (char xdata *)&label_ver_error;
//			error_collect(&label_autoneg, j,(char code *)ptr4,j,0,0);
//			error_collect(&label_autoneg, j,(char code *)ptr4,(SPCTL & 0x03)+0x30,0,0);//print spi speed div
			error_collect(&label_autoneg, j,(char code *)ptr4,(spi_speed & 0x03)+0x30,0,0);//print spi speed div variant 2
		}
		// Bus error
		if(spi_ver_err & 0xf000) error_collect(&label_bus_error, j,0,0,0,0);
		
		// Verify error & error addr if(!verify_err)
//		if(!fFlashOK & !(spi_ver_err & 0xf000) & !(pin_test & 0xff00) && ((pin_test & 0x00FF) == 0xFF))//pin_test)
//		{
//			if(spi_ver_err)
//			if(verify_err)
//			{
////				str_tmp[0] = 8;
////				hex2str(str_tmp, (char *)&verify_err_addr, TRIM_ZEROES | 4);
//				error_collect(&label_ver_error, j,&label_at, j,0,0);
//			}
//		}
		
		////////////////////////////////////////// ok part
		if(overload) error_collect(&label_overload, j,0,0,0,0);
		if(pin_test > 0x00FF) error_collect(&label_chip180, j,0,0,0,0);
		
		for(i = 0; i < 8; i++)
		{
			if((pin_test >> i) & 0x01) continue;
			///////////// | label 1	| param 1 	| label 2			|param 2 | label 3 | param 3
			error_collect(&label_pin, '1' + i,	 &label_nc_text,	j,	 		0, 				0);
		}
		
		// most ugly print verify error address
		if(verify_err)//last visible error
		{
			error_collect(&label_ver_error, j,&label_at, j,&label_hex_0x,0);
		}

		/////////////////////////////////////////////////// Error print
		if(*(int *)err_ptr)// if errors exist
		{
			ptr1 = copy_params(ptr1, &text_label_warning, 0);// 0 --> CRLF
			//////////////////// Converted to array, err_ptr->next not needed
			while(err_ptr)
			{
				if(nullptr == err_ptr->label1) break;
				ptr1 = copy_params(copy_params(copy_params(copy_params(ptr1, \
								err_ptr->label1, err_ptr->param1), \
								err_ptr->label2, err_ptr->param2), \
								err_ptr->label3, err_ptr->param3), \
								0, '\n');// NEW LINE CRLF
				
	//			if(nullptr == err_ptr->next) break;
				err_ptr++;// less than err_ptr = err_ptr->next;
			}
			// most ugly print verify error address
			if(verify_err)
			{
//				ptr1 = copy_params(ptr1, &label_at, j);
				if(!verify_err_addr) ptr1 = text(ptr1-4, "0", 0);
				else ptr1 = hex2str(ptr1-4, (char *)&verify_err_addr, TRIM_ZEROES | 4);
			}
		}
	}
	//
	///////////////////////////////////////////////////// IC Pinout frame render
	{
		ptr1 = copy_params(ptr1, 0, '\n');//new line
		ic_pinout = (IC_PINOUT_IMG *)ptr1;
		ptr1 = copy_labels(ptr1, &label_pinout);
		ptr5[0] = &ic_pinout->pin1_param;
		ptr5[1] = &ic_pinout->pin2_param;
		ptr5[2] = &ic_pinout->pin3_param;
		ptr5[3] = &ic_pinout->pin4_param;
		ptr5[4] = &ic_pinout->pin5_param;
		ptr5[5] = &ic_pinout->pin6_param;
		ptr5[6] = &ic_pinout->pin7_param;
		ptr5[7] = &ic_pinout->pin8_param;
	}
	//
	///////////////////////////////////////////////////// IC ok pins marking
	{
		if(spi | i2c)
		{
			///////////////////////////////////////////////// IC SIZE parameter
			uint2str(&ic_info->mb_param, dwMemorySize >> 20);
			uint2str(&ic_info->kib_param, dwMemorySize >> 10);
			uint2str(&ic_info->bytes_param, dwMemorySize);
			hex2str(&ic_info->hex_param, (char *)&dwMemorySize, TRIM_ZEROES | 4);
//			*(WORD *)icinfo->hex_param = 0x3078;// ugly "0x"
		
			///////////////////////////////////////////////// IC Pinout pins assign
			{
				// method 6 8x6 array <------------- best
				// IC pin labels render
				for(i = 0, j = ' '; i < 8; i++)// -1 bytes in data/ +1-10 in xdata
				{
					if(4 == i) j = '|';
					copy_params(ptr5[i], &ptr6[i*6], j);
				}
				/////////////////////////////////////////////// IC N/C pins marker
				ptr6 = &label_nc;
			}
		}
		/////////////////////////////////////////////////// IC NOT detected marking
		else ptr6 = &label_error;
	}
	//
	///////////////////////////////////////////////////// IC ERROR/NC pins marking
	{
		// error collector & pins marking test
//		pin_test = 0xf8;
		for(i = 0, j = ' '; i < 8; i++)// -1 bytes in data/ +1-10 in xdata
		{
			if(4 == i) j = '|';
			if((pin_test >> i) & 0x01) continue;
			copy_params(ptr5[i], ptr6, j);
		}
	}
	//
	///////////////////////////////////////////////////// Legend
	{
//	ptr1 = copy_labels(ptr1, &text_label_i2c_mem_layout);
		
//	ptr2 = copy_params(&icinfo->ic_type_param, &label_ok, '/');
//	ptr2 = copy_params(ptr2, &label_nc,    '/');
//	copy_params(&ic_pinout->pin1_param, &label_spi_cs,   ' ');
//	copy_params(ptr5[i], &ptr6[i*6], j);
		
//	uint2str(&icinfo->bytes_param, dwMemorySize);
//	hex2str(&icinfo->hex_param, (char *)&dwMemorySize, 4);
		
		// mempool view
//	ptr2 = copy_params(copy_params(hex2str(ptr2, err_ptr, 34), 0, '\0'), 0, '\0');
		
	//error_collect(char code * err_label1, char err_param1, char code * err_label2, char err_param2, char code * err_label3, char err_param3)
		
		///////////// | label 1	| param 1 | label 2		|param 2 | label 3 | param 3
//	error_collect(&label_pin, 	'1', 	 0, 							' ', 0, 					' ');
//	error_collect(&label_pin, 	'2',	 &label_shorted,	' ', &label_pin,  '7');
//	error_collect(&label_pin, 	'3',	 &label_nc_text,	'!', 0, 					' ');
//	error_collect(&label_pin, 	'4',   0, 							' ', 0, 					' ');
//	error_collect(&label_pin, 	'5',   &label_shorted, 	' ', &label_vcc,  ' ');
//	error_collect(&label_pin, 	'6',	 0, 							' ', 0, 					' ');
//	error_collect(&label_pin, 	'7',	 &label_shorted,	' ', &label_pin, 	'2');
//	error_collect(&label_pin, 	'8',	 0, 							' ', 0, 					' ');
//	error_collect(&label_eeprom,' ',	 0, 							' ', 0, 					' ');
//	error_collect(&label_emmc, 	' ', 	 0, 							' ', 0, 					' ');
	}
	//
	///////////////////////////////////////////////////// About info	
	{
		// from new line
		ptr1 = copy_params(copy_params(copy_params(ptr1, \
						0, '\n'), \
						&label_about, '\0'), \
						&label_hello, '\0');
		
		ptr1 = copy_params(copy_params(ptr1, 0, '\n'), &label_host, ' ');
		//
		if('W' == OS_detect || 'w' == OS_detect)
		{
			ptr1 = copy_params(ptr1, &label_windows, '\0');
		}
		// dummy
		
		ptr1 = copy_params(copy_params(ptr1, \
						&label_version, '\0'), \
						&label_build, '\0');
		
		// from new line
		ptr1 = hex2str(copy_params(copy_params(ptr1, \
						0, '\n'), \
						&label_serial_number, ' '), \
						&CHIPID0, 7);
		
		ptr1 = copy_params(copy_params(ptr1, 0, '\n'), &label_copyright, ' ');// from new line
		//
	}
	//
	///////////////////////////////////////////////////// IO test
	{
		// from new line
		ptr1 = copy_params(copy_params(copy_params(copy_params(ptr1, \
						0, '\n'), \
						0, '\n'), \
						&label_io_test1, '\0'), \
						&label_io_test2, '\0');
		
		for(i = 0; i < 8; i++)
		{
			ptr1 = copy_params(uint2str(copy_params(uint2str(copy_params(copy_params(ptr1, \
							&label_pin, '1' + i), 0, ' '), \
							(WORD)pin_level_hi[i]), &label_mv, '/'), \
							(WORD)pin_level_lo[i]), &label_mv, '\0');
		}
	}
	//
	///////////////////////////////////////////////////// DEBUG PRINT //////////////////////////////////
	{
//
	}
		//
	///////////////////////////////////////////////////// End	
}
//
void read_file2()//--- Hex view.txt
{
	char data x, y, z;
	data WORD len;
	char xdata * ptr1 = scsibuf;
	BYTE xdata buf[768];
	char xdata * buf_ptr1 = buf;
//	return;
	memset_xram(scsibuf, ' ', 4096);
	//
//	if(!(spi | i2c))
//	{
//		read_file2_setup(ptr1);
//		return;
//	}
	if((spi | i2c) && !overload)
	{
		if(spi)
		{
//			spi_readsector(0, scsibuf);
			spi_cmd(SPI_READ, (DWORD)0);
			for (len = 0; len < wSectorSize; len++)
			{
				scsibuf[len] = spi_send(0);
			}
			SPI_SS = 1;
		}
		else if(i2c)
		{
			i2c_page_read(0, scsibuf);
		}
		//
		memcpy(buf, scsibuf, 768);
		memset_xram(scsibuf, ' ', 4096);
		len = 0;
//		if(len > dwMemorySize)
//		{
//			len = dwMemorySize;
//		}
		//
		
		ptr1 = text(ptr1, "--HEX--| 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\r\n", 0);//, 58);
		ptr1 = text(ptr1, "-table-| -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --\r\n", 0);//, 58);
		// new ugly hex fucking map
		for(y = 0; y < 48; y++)
		{
			ptr1 = text(hex2str(text(ptr1, "0x", 0), (char *)&len, 2), " : ", 0);
			for(x = 0; x < 16; x++)
			{
//				ptr1 = text(hex2str(ptr1, buf_ptr1++, 1), ' ');
				ptr1 = hex2str(ptr1, buf_ptr1++, 1);
				ptr1++;//spacer
			}
			ptr1 = text(ptr1,": ", 0);
			buf_ptr1 -= 16;
			for(x = 0; x < 16; x++)
			{
				z = *buf_ptr1;
				if(z < 0x20 || z == 0x7f) ptr1 = text(ptr1, ".", 0);
				else ptr1 = text(ptr1, &z, 1);
//				if(*buf_ptr1 < 0x20 || *buf_ptr1 == 0x7f) ptr1 = text(ptr1, ".", 0);
//				else ptr1 = text(ptr1, buf_ptr1, 1);
				buf_ptr1++;
			}
			ptr1 = text(ptr1,"\r\n", 0);
			len += 16;
		}
	}
	ptr1 = text(ptr1, "\r\n\r\n", 0);
	
		//////////////////////////////////////
		//readme part
		if(!fFlashOK)
	{
		text(ptr1, "raccon asshole\r\n", 0);
	}
}
//
/////////////////////////////////////////////////////////*********************** WRITE SECTION ***************************

void write_fakefat()
{
	//
}

//
void write_mem(DWORD addr, int nSize) small
{
	BYTE cnt;
	
	if (nSize > residue) nSize = residue;
  residue -= nSize;
	//
	if(!spi) return;
	//
	if(ic_erase_flag)
	{
		WrittenSectCount++;
		//
		if(!(addr & 0x0000ffff))
		{
			spi_cmd(SPI_BLOCKER64, addr & 0xffff0000);
			SPI_SS = 1;
		}
	}
	//
	usb_write_reg(INDEX, 1);
	usb_write_reg(OUTCSR2, OUTAUTOCLR);
	//
	while(nSize)
	{
			spi_cmd(SPI_PAGEPROG, addr);
			do
			{
				while (USBADR & UBUSY);
				USBADR = OUTCSR1 | UREAD;
				while (USBADR & UBUSY);
			}
			while (!(USBDAT & OUTOPRDY));
			//
			while (USBADR & UBUSY);
			USBADR = OUTCOUNT1 | UREAD;
			while (USBADR & UBUSY);
			cnt = USBDAT;
			addr += cnt;
			nSize -= cnt;
			//
			while(USBADR & UBUSY);
			USBADR = FIFO1 | UREAD;
			while (USBADR & UBUSY);
			//
			while(cnt--)
			{
				spi_send(USBDAT);
				USBADR = FIFO1 | UREAD;
			}
			//
			SPI_SS = 1;
	}
}
//
void write_regs()
{
	//
}
//
void write_file1()//--- Ic Info.txt
{
}
//
void write_file2()//--- Log.txt | Pin test.txt
{
}

void write_file3()//--- Readme.txt | Setup.txt
{
	//
}
