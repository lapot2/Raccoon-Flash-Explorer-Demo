#include "stc.h"
#include "usb.h"
#include "util.h"
#include "scsi.h"
#include "memory.h"
#include "spi.h"
#include "pin_checker.h"
#include "hw_monitor.h"
#include "sfdp.h"

//#define SIZE_16MB			16UL * 1024UL * 1024UL	// Old value of SIZE_64MB
#define SIZE_64MB			(64UL * 1024UL * 1024UL)

BOOL QPI_PRESENT = 0;
char xdata SPI_SECTORER  = 0x20;//0xD7//0xD8
char xdata SPI_CHIPER    = 0xC7;//0x60

DWORD spi_get_size(BYTE * spi_id3);
WORD spi_get_name(char * name, BYTE * spi_id3);

char * winb(char * name, BYTE * spi_id3);
char * mxic(char * name, BYTE * spi_id3);
char * eon(char * name, BYTE * spi_id3);
char * gd(char * name, BYTE * spi_id3);
char * pmc(char * name, BYTE * spi_id3);
char * issi(char * name, BYTE * spi_id3);
char * nansi(char * name, BYTE * spi_id3);
char * sst(char * name, BYTE * spi_id3);
char * st(char * name, BYTE * spi_id3);// and XMIC
char * amic(char * name, BYTE * spi_id3);
//char * spansion(char * name, BYTE * spi_id3);
char * puya(char * name, BYTE * spi_id3);
//char * boya(char * name, BYTE * spi_id3);
//char * intel(char * name, BYTE * spi_id3);
//char * intel2(char * name, BYTE * spi_id3);
//char * esi(char * name, BYTE * spi_id3);
//char * zbit(char * name, BYTE * spi_id3);
//char * xtx(char * name, BYTE * spi_id3);
char * generic(char * name, BYTE * spi_id3);

//vendor id
void spi_detect()
{
	BYTE spi_id3 = spi_JDID_id[2];
//	BYTE xdata * spi_id3 = spi_JDID_id+2;
	//memset(memory_name, "           ", 11);
	memset(memory_name, 0x00, 11);
	switch(spi_JDID_id[0])
	{
		case 0xFF			: break;
		case WINBOND	: winb(memory_name, &spi_id3);		break;
		case MXIC			: mxic(memory_name, &spi_id3);		break;
		case EON			: eon(memory_name, &spi_id3);			break;
		case GD				: gd(memory_name, &spi_id3);			break;
		case PMC			: pmc(memory_name, &spi_id3);			break;
		case ISSI			: issi(memory_name, &spi_id3);		break;
		case NANSI		: nansi(memory_name, &spi_id3);		break;
		case SST			: sst(memory_name, &spi_id3);			break;
		case ST				: st(memory_name, &spi_id3);			break;// and XMIC	
		case AMIC			: amic(memory_name, &spi_id3);		break;	
//		case ATMEL		: atmel(memory_name, &spi_id3);		break;		
//		case SPANSION	: spansion(memory_name, &spi_id3);	break;	
		case PUYA			: puya(memory_name, &spi_id3);		break;// SY25Q16/P25D16 85:60:15/PY25Q64 85:20:17
//		case BOYA			: boya(memory_name, &spi_id3);		break;// No HOLD Pin BY25D80 68:40:14
//		case INTEL		: intel(memory_name, &spi_id3);		break;		
//		case INTEL2		: intel2(memory_name, &spi_id3);		break;// MK25Q32 D8:40:16
//		case ESI			: esi(memory_name, &spi_id3);		break;		
//		case NTRCS		: ntrcs(memory_name, &spi_id3);		break;	
//		case ZBIT			: zbit(memory_name, &spi_id3);		break;
//		case XTX			: zbit(memory_name, &spi_id3);		break;
		default				: 
		{
			if(spi_JDID_id[0]) hex2str(memory_name, spi_JDID_id, 3);
			else generic(memory_name, &spi_id3);
			//
			break;
		}
	}
	//
//	sfdp_present = 0;//debug
	if(sfdp_present)
	{
		dwMemorySize = sfdp_mem_size;
		(BYTE)dwMemorySize &= 0x7f; 
		dwMemorySize++;
		dwMemorySize = dwMemorySize >> 3;//sfdp_mem_size /= 8;
	}
	else 
		dwMemorySize = spi_get_size(&spi_id3);
	if(dwMemorySize > SIZE_64MB)
	{
		dwMemorySize = 0;
		dwSectorCount = 0;
		return;
	}
	dwSectorCount = dwMemorySize / wSectorSize;
	//
}

char * generic(char * name, BYTE * spi_id3)// IF RDID ONLY
{
	char xdata str[11] = {"OLD_IC"};
	char xdata len = strlen(str);
	str[9] = 0;
	//
	spi_get_name(str + len, spi_id3);
	len = strlen(str);
	memcpy(name, str, len);
	//
	*spi_id3 = 0;
	//
	return name + len;
}

//device id
//memcpy(memory_name, "W25Q10     ", sizeof(memory_name));
//memcpy(memory_name+4, ctoa((WORD)size), 3);
//
DWORD spi_get_size(BYTE * spi_id3)
{
	DWORD size = 0;
	if(0x10 == *spi_id3)// || (0x9D == spi_JDID_id[1] && 0x20 == spi_JDID_id[2]))// for PM25LD512 7F:9D:20
		return 1UL << 16;
	else
		size = 1UL << ((*spi_id3 - 1) & 0x0F);
	return size << 17;
}
//
WORD spi_get_name(char * name, BYTE * spi_id3)
{
	WORD size = 0;
	size = 1UL << ((*spi_id3 - 1) & 0x0F);
	
	if(0x10 == *spi_id3)// || (0x9D == spi_JDID_id[1] && 0x20 == spi_JDID_id[2]))// for PM25LD512 7F:9D:20
	{
		memcpy(name, "512", 3);
		return 0;
	}
	else if(size < 16) 
		size *= 10;
	
	uint2str(name, (WORD)size);
//	if(size < 16) 
//		memcpy(name+1, "0", 1);
	return size;
}
//
//**********************************************************
char * winb(char * name, BYTE * spi_id3)
{
	char xdata str[11] = {"W25"};//W25Q256FV
	char xdata len = strlen(str);
	str[9] = 0;
	//
	if(0x20 == spi_JDID_id[1])
	{
		str[len] = 'P';
	}
	else if(0x30 == spi_JDID_id[1])
	{
		str[len] = 'X';
	}
	else if(0x40 == spi_JDID_id[1])
	{
		str[len] = 'Q';
	}
	else if(0x60 == spi_JDID_id[1])
	{
		str[len] = 'Q';
	}
//	else if(0x80 == spi_JDID_id[1])// for W25QxxJyyM / with DTR mode
//	{
//		str[len] = 'Q';
//		double_transfer_rate = 1;
//	}
	else// if(spi_JDID_id[1])
	{
		str[len] = '_';
	}
	len++;
	//
	spi_get_name(str + len, spi_id3);
	len = strlen(str);
	//
	if(0x40 == spi_JDID_id[1])
	{
		memcpy(str+len, "FV", 2);
		if(!QPI_PRESENT && spi_JDID_id[2] < 0x17)
			memcpy(str+len, "BV", 2);
	}
	else if(0x60 == spi_JDID_id[1])
	{
		if(spi_JDID_id[2] < 0x14)
			memcpy(str+len, "EW", 2);
		else
			memcpy(str+len, "FW", 2);
	}
	//	
	len = strlen(str);
	memcpy(name, str, len);
	//
	return name + len;
}
//
char * mxic(char * name, BYTE * spi_id3)
{
	char xdata str[11] = {"MX25L"};
	char xdata len = strlen(str);
	str[9] = 0;
//	if(sfdp_present)
//		memcpy(str, "MX25V", 5);
	//
	if(0x25 == spi_JDID_id[1])
	{
		str[4] = 'U';
	}
	else if(0x75 == spi_JDID_id[1])
	{
		str[2] = '7';
		str[3] = '7';
		str[4] = 'U';
	}
	else// if(spi_JDID_id[1] != 0x20)
	{
		str[4] = '_';
	}
	//
	spi_get_name(str + len, spi_id3);
	len = strlen(str);
	memcpy(name, str, len);
	//
	return name + len;
}
//
char * eon(char * name, BYTE * spi_id3)
{
	char xdata str[11] = {"EN25"};
	char xdata len = strlen(str);
	str[9] = 0;
	//
//	if(0x30 == spi_JDID_id[1])
//	{
//		str[len] = 'B';
//	}
//	else 
		if(0x31 == spi_JDID_id[1])
	{
		str[len] = 'F';
	}
	else if(0x51 == spi_JDID_id[1])
	{
		str[len] = 'T';
	}
	else if(0x70 == spi_JDID_id[1] || 0x30 == spi_JDID_id[1])
	{
		str[len] = 'Q';
	}
	else// if(spi_JDID_id[1])
	{
		str[len] = '_';
	}
	len++;
	//
	spi_get_name(str + len, spi_id3);
	len = strlen(str);
//	if(0x30 == spi_JDID_id[1])
	if(QPI_PRESENT)
	{
		memcpy(str+len, "B", 1);
	}
	len = strlen(str);
	memcpy(name, str, len);
	//
	return name + len;
}
//
char * gd(char * name, BYTE * spi_id3)
{
	char xdata str[11] = {"GD25"};
	char xdata len = strlen(str);
	str[9] = 0;
	//
	spi_write_reg(SPI_WRSR1, spi_regs[0], (spi_regs[1] & ((BYTE)0 << 1)));
		while(spi_isbusy());
		if((spi_read_reg(SPI_RDSR2) & 0x02))
		{
			spi_write_reg(SPI_WRSR1, spi_regs[0], (spi_regs[1] & ((BYTE)0 << 1)));
			while(spi_isbusy());
		}
		//
	if(0x02 == (spi_regs[1] & 0x02))
	{
		spi_write_reg(SPI_WRSR1, spi_regs[0], 0x00);
		while(spi_isbusy());
		if((spi_read_reg(SPI_RDSR2) & 0x02) != 0x00)
		{
			spi_write_reg(SPI_WRSR2, 0x00, 0xff);
			while(spi_isbusy());
			str[9] = 1;
		}
		else 
			str[9] = 2;
		if((spi_read_reg(SPI_RDSR2) & 0x02) != 0x00)
			str[9] = 3;
	}
			//
	if(0x40 == spi_JDID_id[1])
	{
		if(3 != str[9])
			str[len] = 'Q';
		else
			str[len] = 'B';
	}
	else if(0x60 == spi_JDID_id[1])
	{
		str[len] = 'L';
		len++;
		//
		if(3 != str[9])
			str[len] = 'Q';
		else
			str[len] = 'B';
	}
	if(1 == str[9]) spi_write_reg(SPI_WRSR2, spi_regs[1], 0xff);
	else if(2 == str[9]) spi_write_reg(SPI_WRSR1, spi_regs[0], spi_regs[1]);
	while(spi_isbusy());
	str[9] = 0;
	if(spi_JDID_id[1] != 0x40 && spi_JDID_id[1] != 0x60 )
	{
		str[len] = '_';
	}
	len++;
	//
	spi_get_name(str + len, spi_id3);
	len = strlen(str);
	memcpy(name, str, len);
	//memcpy(name+len, " ", 2);
	//
	return name + len;
}
//
char code pmc_remap[] = {0x20,0x10,0};
char * pmc(char * name, BYTE * spi_id3)
{
	char xdata str[11] = {"A25L"};
	char xdata len = strlen(str);
	str[9] = 0;
	//
	if(0x9D == spi_JDID_id[1])
	{
		if(0x20 == spi_JDID_id[2])// PM25LD512 7F:9D:20 BUG!!!
		{
			memcpy(str, "PM25LD", 6);
			*spi_id3 = spi_id_remap(*spi_id3, pmc_remap);
		}
		else memcpy(str, "PM25LD0", 7);
		len = strlen(str);
	}
	else if(0x37 == spi_JDID_id[1])
	{
		len = strlen(str);
	}
	else// if(spi_JDID_id[1])
	{
		str[len] = '_';
	}
	//
	spi_get_name(str + len, spi_id3);
	len = strlen(str);
	memcpy(name, str, len);
	//
	SPI_SECTORER = 0xD7;
	SPI_CHIPER = 0x60;
	//
	return name + len;
}
//
char * issi(char * name, BYTE * spi_id3)// and old PMC PM25LV512
{
	char xdata str[11] = {"IS25W"};
	char xdata len = strlen(str);
	str[9] = 0;
	//
	if(0x11 == spi_JDID_id[1] || 0x12 == spi_JDID_id[1])
	{
		str[len] = 'Q';
	}
	else if(0x70 == spi_JDID_id[1])
	{
		str[len] = 'P';
	}
	else// if(spi_JDID_id[1])
	{
		str[len] = '_';
	}
	len++;
	str[len] = '0';
	len++;
	//
	spi_get_name(str + len, spi_id3);
	len = strlen(str);
	memcpy(name, str, len);
	//
	return name + len;
}
//
char * nansi(char * name, BYTE * spi_id3)
{
	char xdata str[11] = {"NS25S"};
	char xdata len = strlen(str);
	str[9] = 0;
	if(spi_JDID_id[1] != 0x30)
	{
		str[4] = '_';
	}
	//
	spi_get_name(str + len, spi_id3);
	len = strlen(str);
	memcpy(name, str, len);
	//
	return name + len;
}
//
char code sst_remap[] = {0x8C,0x12,0x8D,0x13,0x8E,0x14,0x41,0x15,0x4A,0x16,0x4B,0x17,0x4C,0x18,0};
char * sst(char * name, BYTE * spi_id3)
{
	char xdata str[11] = {"SST25VF0"};
	char xdata len = strlen(str);
	str[9] = 0;
	//
	if(0x25 == spi_JDID_id[1])
	{
		*spi_id3 = spi_id_remap(*spi_id3, sst_remap);
		
		if(0x4C == *spi_id3)
		{
			*spi_id3 = 0x18;
			memcpy(str, "SST25VF", 7);
			len--;
		}
	}
	else// if(spi_JDID_id[1])
	{
		str[6] = '_';
		*spi_id3 = 0x00;
	}
	//
	spi_get_name(str + len, spi_id3);
	len = strlen(str);
	memcpy(name, str, len);
	//
//	SPI_CHIPER = 0x60;
	SPI_CHIPER = 0xFF;//erase disabled
	//
	*spi_id3 = 0;// SST not supported
	//
	return name + len;
}
//
char * st(char * name, BYTE * spi_id3)// and XMIC /////////////// XMIC ident MUST BE REVIEW & OPTIMIZE /////////////////////
{
	char xdata str[11] = {"M25P"};
	char xdata len = strlen(str);
	str[9] = 0;
	//
	if(0x20 == spi_JDID_id[1])
	{
		//str[len] = 'P';
	}
	else if(0x71 == spi_JDID_id[1])
	{
		str[len] = 'X';
	}
	else if(0x80 == spi_JDID_id[1])
	{
		str[len] = 'E';
	}
	////////////////////////////////// XMIC ident MUST BE REVIEW & OPTIMIZE /////////////////////
	else if(0x70 == spi_JDID_id[1])
	{
		memcpy(str, "XM25QH", 6);
		len = 5;
	}
	////////////////////////////////// MUST BE ADDED FROM USER REQUEST //////////////////////////
	else if(0x43 == spi_JDID_id[1])
	{
		memcpy(str, "XM25RH", 6);
		len = 5;
	}
	else if(0x44 == spi_JDID_id[1] && V18)
	{
		memcpy(str, "XM25RU", 6);
		len = 5;
	}
	else if(0x41 == spi_JDID_id[1] && V18)
	{
		memcpy(str, "XM25QU", 6);
		len = 5;
	}
	else if(0x38 == spi_JDID_id[1] && V18)
	{
		memcpy(str, "XM25QU", 6);
		len = 5;
	}
	else// if(spi_JDID_id[1])
	{
		str[len] = '_';
	}
	len++;
	//
	spi_get_name(str + len, spi_id3);
	len = strlen(str);
	//	
	memcpy(name, str, len);
	//
	if(0x70 != spi_JDID_id[1])	
		SPI_SECTORER = 0xD8;
	return name + len;
}
//
char * amic(char * name, BYTE * spi_id3)
{
	char xdata str[11] = {"A25L"};
	char xdata len = strlen(str);
	str[9] = 0;
	//
	if(0x30 == spi_JDID_id[1])
	{
		if(spi_JDID_id[1] > 0x10)
			str[len] = '0';
	}
	else// if(spi_JDID_id[1])
	{
		str[len] = '_';
	}
	len = strlen(str);
	//
	spi_get_name(str + len, spi_id3);
	len = strlen(str);
	//	
	memcpy(name, str, len);
	//
	return name + len;
}
//
//////////////////////////////////////////////// HIDDEN IC's

/*char * atmel(char * name, BYTE * spi_id3)
{
	//
	SPI_CHIPER = 0xFF;//0x60;
	//
	return name + len;
}*/
//
/*char * spansion(char * name, BYTE * spi_id3)
{
	//
	SPI_SECTORER = 0xD8;
	return name + len;
}*/
//
char * puya(char * name, BYTE * spi_id3)// SY25Q16/P25D16 85:60:15
{
	char xdata str[11] = {"PY25Q"};
	char xdata len = strlen(str);
	str[9] = 0;
	//
//	if(spi_JDID_id[1] != 0x60)
//	{
//		str[4] = '_';
//	}
	//
	spi_get_name(str + len, spi_id3);
	len = strlen(str);
	memcpy(name, str, len);
	//
	SPI_SECTORER = 0xD8;
	return name + len;
}
//
/*char * intel(char * name, BYTE * spi_id3)
{
	//
	SPI_SECTORER = 0xD8;
	return name + len;
}*/
//
/*char * intel2(char * name, BYTE * spi_id3)// MK25Q32 D8:40:16
{
	//
	SPI_SECTORER = 0xD8;
	return name + len;
}*/
//
/*char * esi(char * name)
{
	//
	return name + len;
}*/
//
