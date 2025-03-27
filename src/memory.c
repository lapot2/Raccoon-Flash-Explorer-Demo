
#include "main.h"
#include <stdlib.h>
#include "stc.h"
#include "memory.h"
#include "scsi.h"
#include "util.h"
#include "spi.h"
#include "i2c.h"
#include "pin_checker.h"
#include "hw_monitor.h"


DWORD dwTotalMemSize;
DWORD dwTotalSectorCount;

BYTE ClusterSize;

DWORD data dwMemorySize = 0;
const WORD wSectorSize = 4 * 1024UL;
DWORD dwSectorCount = 0;
uchar xdata memory_name[11] = {"No Flash   "};
///////////////////////////////"-----------"

BOOL spi = 0;
BOOL i2c = 0;

BOOL ic_erase_flag = 0;

BOOL fFlashOK;//static BOOL fFlashOK;

DWORD flash_start_sector_offset = 7;//8;

FAT16PBR xdata FakePart1;
FS_ROOT_ENTRY xdata fs_entry;
FS_ROOT_ENTRY xdata fs_entry1;
FS_ROOT_ENTRY xdata fs_entry2;
FS_ROOT_ENTRY xdata fs_entry3;
FS_ROOT_ENTRY xdata fs_entry4;
FS_ROOT_ENTRY xdata fs_entry5;
void FakeFAT_init()
{
	memcpy(FakePart1.raw_PBR, &FAT_PBR_Data, 512);// PBR
	memcpy(fs_entry.raw_SFN, &rawRoot_entry0, 0x20);// Root entry

	memcpy(fs_entry1.raw_SFN, &rawRoot_entry0 + 0x20, 0x20);// File entry 1
	//
	if(dwMemorySize > (8192UL * 1024UL))
		ClusterSize = dwMemorySize / (8192 * 1024UL);
	else 
		ClusterSize = 1;
	FakePart1.PBR.ClusterSize = ClusterSize;
	//
	fs_entry5 = fs_entry4 = fs_entry3 = fs_entry2 = fs_entry1;
	fs_entry5.SFN.FileSize = reverse4((DWORD)0x5F);//reverse4(fs_entry5.SFN.FileSize)
	dwTotalMemSize = dwMemorySize + reverse4(fs_entry2.SFN.FileSize) + reverse4(fs_entry3.SFN.FileSize) + reverse4(fs_entry4.SFN.FileSize) + reverse4(fs_entry5.SFN.FileSize);
	dwTotalSectorCount = dwSectorCount + (4 * ClusterSize);

}
//
void entry_save(BYTE * SecBuff, uint16_t FATSecNum, uint16_t NewEntryVal)
{
    if (FATSecNum & 1) 
    {    // Odd entry
        SecBuff[FATSecNum] = (SecBuff[FATSecNum] & 0x0F) | (NewEntryVal << 4);
        SecBuff[FATSecNum + 1] = NewEntryVal >> 4;
    } 
    else 
    {        // Even entry
        SecBuff[FATSecNum] = NewEntryVal;
        SecBuff[FATSecNum + 1] = (SecBuff[FATSecNum + 1] & 0xF0) | ((NewEntryVal >> 8) & 0x0F);
    }
}

//
void fat_make(BYTE * pdat)//(DWORD filesize, WORD i) ...to future...
{
	//
  BYTE reserved = 2;
	WORD i;
	WORD pos_count;
	//
	pos_count = (((WORD)dwSectorCount / FakePart1.PBR.ClusterSize) + reserved); // first file | flash
  //////////////////////////////////
    // >> ClusterSize
		for(i = 0; pos_count > i; i++)
    {
      if(i && !(i & 1))
          pdat++;
          
      if(!i)
        entry_save(pdat, i, 0xff8);
        
			else if(reserved > i || pos_count - 1 == i)
        entry_save(pdat, i, 0xfff);
			
			else if(reserved <= i)
        entry_save(pdat, i, 1 + i);//i + offset);
    }
}

//
extern BYTE idata spi_speed;
//
void FakeFat_readsector(DWORD lba, BYTE * pdat)
{
	//
	WORD i = 0;
	lba+=1;
	switch(lba)
	{
		case 0:// MBR
		{
			memset_xram(scsibuf, 0x00, 4096);
			memcpy(scsibuf, &FAT_MBR_Data, 512);
			FakePart1.PBR.NumSectSmallParts = reverse2((WORD)dwTotalSectorCount + flash_start_sector_offset-1);// sectors with ad files
			memcpy(&scsibuf[0x1ca], &FakePart1.PBR.NumSectSmallParts, 4);
			break;
		}
		case 1:// PBR
		{
			memset_xram(scsibuf, 0x00, 4096);
			
			FakePart1.PBR.NumSectSmallParts = 0;
			FakePart1.PBR.NumSectSmallParts = reverse2((WORD)dwTotalSectorCount + flash_start_sector_offset-1);// sectors with ad files
			memcpy(pdat, FakePart1.raw_PBR, 512);

			break;
		}
		case 2:// FAT table
		{
			memset_xram(scsibuf, 0xff, 4096);
			fat_make(scsibuf);
			break;
		}
		case 3:// FAT table copy
		{
			break;
		}
		case 4:// Root dir 1st sector
		{
			memset_xram(scsibuf, 0x00, 4096);
			//
			if(overload)
			{
				memcpy(memory_name, "OVERLOAD   ", 11);
				dwMemorySize = 0;
			}
			else if(0x00FF < pin_test)
			{
				memcpy(memory_name, "Chip 180   ", 11);
				dwMemorySize = 0;
			}
			else if(!fFlashOK & verify_err)// & spi)
			{
				memcpy(memory_name, "IC Dead    ", 11);
				dwMemorySize = 0;
			}
			//
			memcpy(pdat, fs_entry.raw_SFN, 0x20);// first root entry
			memcpy(pdat, memory_name, sizeof(memory_name));// this entry name
			//
			memcpy(pdat+0x20, &fs_entry1.raw_SFN, 0x20);
			//
			if(0x00 == memory_name[10])// redetect FS failed without this
				memset(memory_name+strlen(memory_name), 0x20, 11-strlen(memory_name));// extension bug without this
			//
			memcpy(pdat+0x20, memory_name, sizeof(memory_name));
			if(dwMemorySize)
			{
				memcpy(pdat+0x28, "BIN", 3);
				//memcpy(pdat+0x28, "MP3", 3);
			}
			
			*(DWORD * )&scsibuf[0x3c] = reverse4(dwMemorySize);// binary file size
			//
			fs_entry2.SFN.StartClusterLow = reverse2(((WORD)dwSectorCount / FakePart1.PBR.ClusterSize) + 2);//3);
			fs_entry3.SFN.StartClusterLow = reverse2(((WORD)dwSectorCount / FakePart1.PBR.ClusterSize) + 3);//4);
			fs_entry4.SFN.StartClusterLow = reverse2(((WORD)dwSectorCount / FakePart1.PBR.ClusterSize) + 4);//5);
			//
			fs_entry5.SFN.StartClusterLow = reverse2(((WORD)dwSectorCount / FakePart1.PBR.ClusterSize) + 5);//5);
			//
			memcpy(&fs_entry2.raw_SFN, "IC INFO TXT", 11);
			memcpy(pdat+0x40, &fs_entry2.raw_SFN, 0x20);

			{
#ifdef SFDP_TEST
				if(sfdp_present)//////////////////////////////// SFDP test output
				{
					memcpy(&fs_entry3.raw_SFN, "SFDP    BIN", 11);
					memcpy(pdat+0x60, &fs_entry3.raw_SFN, 0x20);
				}
				else
#endif
				{
					memcpy(&fs_entry3.raw_SFN, "HEX VIEWTXT", 11);
					memcpy(pdat+0x60, &fs_entry3.raw_SFN, 0x20);
				}
			}
			memcpy(&fs_entry4.raw_SFN, "SUPPORT HTM", 11);
			memcpy(pdat+0x80, &fs_entry4.raw_SFN, 0x20);
			break;			
		}/*
		case 4:// Root dir 2nd sector
		{
			break;
		}
		case 5:// Reserved 1st sector
		{
			break;
		}
		case 6:// Reserved 2nd sector
		{
			break;
		}
		case 7:// FS 1st sector
		{
			break;
		}
		case 8:// FS 2nd sector
		{
			break;
		}
		case 9:// FS 3rd sector
		{
			break;
		}
		case 10:// FS 4th sector
		{
			break;
		}*/
		default:
		{
			memset_xram(scsibuf, 0x00, 4096);
			break;
		}
	};
}



