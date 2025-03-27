/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/* --- Web: www.STCMCUDATA.com  ---------------------------------------*/
/* --- BBS: www.STCAIMCU.com  -----------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "stc.h"
#include "usb.h"
#include "usb_desc.h"
#include "scsi.h"
#include "util.h"
#include "memory.h"
#include "spi.h"
#include "i2c.h"
#include "pin_checker.h"
#include "scsi_rw_parser.h"
#include "sfdp.h"
#include "HTML_PAGE.H"
//#include <string.h>
#include "main.h"
//#define FAT_DEBUG

//#define LBA                 (MEMORYSIZE/SECTORSIZE - 1)

char OS_detect = 0x77;//'w';
BOOL scsi_ready = 1;
BOOL verify_err = 0;
DWORD data verify_err_addr = 0;

static CBW data cbw;
static CSW data csw;
static BYTE data status;
static BYTE data sensekey;//USED in scsi_request_sense()
static BYTE data senseasc;//USED in scsi_request_sense()
DWORD data residue;// static 
BYTE xdata scsibuf[4096] _at_ 0x1000;
BYTE xdata str_tmp[16];
//static BYTE xdata version[12] = {"\r\nVer:1.0b\r\n"};

DWORD WrittenSectCount = 0;

void scsi_process()
{
    BYTE cnt;
    BYTE key;
    BYTE asc;
	
    cnt = usb_bulk_intr_out((BYTE *)&cbw, 1);
    if ((sizeof(CBW) == cnt) && (CBW_SIGNATURE == cbw.dCBWSignature))
    {
        residue = reverse4(cbw.dCBWDataLength);
        status = SCSI_PASSED;
//				status = SCSI_BUSY;
			
//		USED in scsi_request_sense()
        key = SENSE_NOSENSE;
        asc = 0;
       
        switch (cbw.bScsiOpcode)
        {
        case SCSI_TESTUNITREADY:
            if (!scsi_test_unit_ready())
            {
                status = SCSI_FAILED;
                key = SENSE_NOTREADY;
                asc = ASC_MEDIUMNOTPRESENT;
							scsi_ready = 1;
            }
            break;
        case SCCI_STARTSTOPUNIT:
            scsi_start_stop_unit();
            break;
        case SCSI_MEDIAREMOVAL:
            scsi_media_removal();
            break;
        case SCSI_INQUIRY:
            scsi_inquiry();
            break;
        case SCSI_READCAPACITY10:
            scsi_read_capacity10();
            break;
        case SCSI_FORMATCAPACITY:
            scsi_format_capacity();
            break;
        case SCSI_REQUESTSENSE:
            scsi_request_sense();
            break;
        case SCSI_MODESENSE6:
            scsi_mode_sense6();
            break;
        case SCSI_MODESENSE10:
            scsi_mode_sense10();
            break;
        case SCSI_READ10:
            scsi_read10();
            break;
        case SCSI_WRITE10:
            scsi_write10();
            break;
        case SCSI_VERIFY10:
            scsi_verify10();
            break;
        default:
//					skip_command:
            status = SCSI_FAILED;
            key = SENSE_ILLEGALREQUEST;
            asc = ASC_INVALIDCOMMAND;
            break;
        }
//		USED in scsi_request_sense()
        sensekey = key;
        senseasc = asc;
        
        csw.dCSWSignature = CSW_SIGNATURE;
        csw.dCSWTag = cbw.dCBWTag;
        csw.dCSWDataResidue = reverse4(residue);
        csw.bCSWStatus = status;
        
        scsi_in(NULL, 0);
    }
    else
    {
        usb_write_reg(INCSR1, INSDSTL);
        usb_write_reg(OUTCSR1, OUTSDSTL);
    }
}

void scsi_in(BYTE *pData, int nSize)
{
    BYTE cnt;
    
    if (nullptr == pData)
    {
        pData = (BYTE *)&csw;
        nSize = sizeof(CSW);
    }
    else
    {
        if (nSize > residue)
        {
            nSize = residue;
        }
        residue -= nSize;
    }
    
    while (nSize)
    {
        cnt = EP1IN_SIZE;
        if (nSize < EP1IN_SIZE) cnt = nSize;
        
        while (usb_bulk_intr_in_busy());
        usb_bulk_intr_in(pData, cnt, 1);
        
        nSize -= cnt;
        pData += cnt;
    }
}

void scsi_out(BYTE *pData, int nSize)
{
    BYTE cnt;
    
    if (nSize > residue)
    {
        nSize = residue;
    }
    residue -= nSize;
    
    while (nSize)
    {
        while (!usb_bulk_intr_out_ready());
        cnt = usb_bulk_intr_out(pData, 1);
        
        nSize -= cnt;
        pData += cnt;
    }
}

BOOL scsi_test_unit_ready()
{
    //return 1;//memory_check();  // Permanent ready
	return scsi_ready;
}

void scsi_start_stop_unit()
{
}

void scsi_media_removal()
{
}

void scsi_inquiry()
{
    static char code INQUIRY_DATA[36] =
    {
				0x00,                   //0x00: SBC-4 DeviceType(Direct-access device);
				//0x0E,                   //0x0E: RBC DeviceType(Simplified Direct-access device);
        0x80,                   //RemovableMedia(Removable);
        0x00,                   //Versions(ANSI X3.131:1994);
        0x00,                   //ResponseDataFormat(Standard);
        0x1f,                   //AdditionalLength(31);
        0x00,0x00,0x00,         //Reserved;
        
				'R','a','c','c','o','o','n',' ',
        'F','l','a','s','h',' ','E','x','p','l','o','r','e','r',' ',' ',
        'D','E','M','O',
    };
		//P70 = 0;
    scsi_in(INQUIRY_DATA, sizeof(INQUIRY_DATA));
}

void scsi_read_capacity10()
{
    static char code CAPACITY_DATA[8] =
    {
        0x00,0x00,0x00,0x00,    //LastLogicalBlockAddress(LBA)
        0x00,0x00,0x00,0x00,    //BytesPerBlock(SECTORSIZE);
    };

    memcpy(scsibuf, CAPACITY_DATA, sizeof(CAPACITY_DATA));
    //*(DWORD *)&scsibuf[0] = (dwSectorCount + flash_start_sector_offset) - 1;
		*(DWORD *)&scsibuf[0] = (dwTotalSectorCount + flash_start_sector_offset) - 1;// sectors with ad files
    *(WORD *)&scsibuf[6] = wSectorSize;
    
    scsi_in(scsibuf, sizeof(CAPACITY_DATA));
}

void scsi_format_capacity()
{
    static char code FORMATCAPACITY_DATA[12] =
    {
        0x00,0x00,0x00,0x08,    //CapacityListLength(8)
        0x00,0x00,0x00,0x00,    //NumberOfBlocks(LBA)
        0x02,                   //DescriptorTypes(Formatted Media)
        0x00,0x00,0x00,         //BytesPerBlock(SECTORSIZE);
    };

    memcpy(scsibuf, FORMATCAPACITY_DATA, sizeof(FORMATCAPACITY_DATA));
    //*(DWORD *)&scsibuf[4] = (dwSectorCount + flash_start_sector_offset) - 1;
		*(DWORD *)&scsibuf[4] = (dwTotalSectorCount + flash_start_sector_offset) - 1;// sectors with ad files
    *(WORD *)&scsibuf[9] = wSectorSize;
    
    scsi_in(scsibuf, sizeof(FORMATCAPACITY_DATA));
}

void scsi_request_sense()
{
    static char code REQUESTSENSE_DATA[18] =
    {
        0x70,                   //ErrorCode(0x70);
        0x00,                   //Reserved;
        0x00,                   //SenseKey;
        0x00,0x00,0x00,0x00,    //Information;
        0x0a,                   //AdditionalSenseLength(10);
        0x00,0x00,0x00,0x00,    //Reserved;
        0x00,                   //ASC;
        0x00,                   //ASCQ;
        0x00,0x00,0x00,0x00,    //Reserved;
    };

    memcpy(scsibuf, REQUESTSENSE_DATA, sizeof(REQUESTSENSE_DATA));
    scsibuf[2] = sensekey;
    scsibuf[12] = senseasc;

    scsi_in(scsibuf, sizeof(REQUESTSENSE_DATA));
}

void scsi_mode_sense6()
{
    static char code MODESENSE6_DATA[4] =
    {
        0x03,0x00,0x00,0x00,
    };

    scsi_in(MODESENSE6_DATA, sizeof(MODESENSE6_DATA));
}

void scsi_mode_sense10()
{
    static char code MODESENSE10_DATA[8] =
    {
        0x00,0x06,0x00,0x80,
        0x00,0x00,0x00,0x00,
    };
		//P70 = 0;
    scsi_in(MODESENSE10_DATA, sizeof(MODESENSE10_DATA));
}

//
//
void scsi_read10()
{
    DWORD data lba;
		DWORD data SectorNumber;
		WORD  data flash_offset = flash_start_sector_offset * wSectorSize;
  //
	blinker(10, 2);// P70 LED blink
	//
    lba = *(DWORD *)&cbw.ScsiParam[1];
    SectorNumber = lba * wSectorSize;
		
    while (residue)
    {
			
			if(lba >= flash_start_sector_offset && lba < flash_start_sector_offset + dwSectorCount)
			{
				if(spi)
				{
					read_mem(SectorNumber - flash_offset, wSectorSize);
					goto next_sector;
				}
				else if(i2c)
				{
					i2c_page_read(SectorNumber - flash_offset, scsibuf);
					i2c_page_verify(SectorNumber - flash_offset, scsibuf);
				}
			}
			else if(lba < flash_start_sector_offset)
			{
				FakeFat_readsector(lba, scsibuf);
			}
			else if(lba == flash_start_sector_offset + dwSectorCount)//----------------------- Ic Info.txt
			{
				read_file1();
			}
			else if(lba == flash_start_sector_offset + dwSectorCount + (ClusterSize * 1))//--- Hex view.txt | Readme.txt | Setup.txt
			{
#ifdef SFDP_TEST
			// dummy
#endif
				read_file2();
			}
			else if(lba == flash_start_sector_offset + dwSectorCount + (ClusterSize * 2))//--- Support.htm
			{
				memset_xram(scsibuf, 0x20, 4096);
				
				fast_memcpy(fast_memcpy(fast_memcpy(scsibuf, \
				html_link1, sizeof(html_link1)-1, OUT_FWD|IN_FWD),\
				html_link2, sizeof(html_link2)-1, OUT_FWD|IN_FWD),\
				html_link3, sizeof(html_link3)-1, OUT_FWD|IN_FWD);
			}
			else
			{
				memset_xram(scsibuf, 0x20, 4096);
				memcpy(scsibuf, "Unused sector", 13);
			}
        scsi_in(scsibuf, wSectorSize);
			next_sector:
        SectorNumber += wSectorSize;
				lba++;
    }
}
//
static BYTE deleted_file_marker_adr = 0x20;// 0xE5 on firsf byte fileentry
void scsi_write10()
{
		DWORD data lba;
		DWORD data SectorNumber;
		WORD  data flash_offset = flash_start_sector_offset * wSectorSize;
	
		DWORD FakeLBA;
	
    lba = *(DWORD *)&cbw.ScsiParam[1];
    SectorNumber = (lba * wSectorSize) - flash_offset;
		FakeLBA = lba - flash_start_sector_offset;
		//
    while (residue)
    {
			P70 = !P70;// LED off
			//************************************ Write flash

			if(spi && lba >= flash_start_sector_offset && lba < flash_start_sector_offset + dwSectorCount)//service data 0-x sectors
			{
#ifdef FAT_DEBUG
				// dummy
#else
				//
				if('W' == OS_detect || 'w' == OS_detect)
				{
					write_mem(SectorNumber, wSectorSize);
				}
				else
				{
					scsi_out(scsibuf, wSectorSize);
				}
#endif //FAT_DEBUG
					goto next_sector;
			}
			else
				scsi_out(scsibuf, wSectorSize);
			
			if(i2c && lba >= flash_start_sector_offset && lba < flash_start_sector_offset + dwSectorCount)
			{
				
				if('W' == OS_detect || 'w' == OS_detect)
				{
					i2c_page_write(SectorNumber, scsibuf);
					i2c_page_verify(SectorNumber, scsibuf);
				}
			}
			
			//************************************ Write service data
			if(lba < flash_start_sector_offset)
			{
#ifdef FAT_DEBUG
					// dummy
#else
				OS_detect = 'W';// dummy
				//
				if(3 == lba && spi)// root dir
				{
					if(ic_erase_flag && (WrittenSectCount == dwSectorCount))// autoreset after ic writing
					{
						ic_erase_flag = 0;
						scsi_ready = 0;
						WrittenSectCount = 0;
						if(spi_regs[0] > 0x03)
						{
							spi_write_reg(SPI_WRSR1, spi_regs[0], 0xFF);
							while(spi_isbusy());
						}
					}
					if(0xE5 == scsibuf[deleted_file_marker_adr])// startup ic erase flag after deleting
					{
						deleted_file_marker_adr = 0x00;// reset the erase marker
						ic_erase_flag = 1;
						WrittenSectCount = 0;
						if(spi_regs[0] > 0x03)
						{
							spi_write_reg(SPI_WRSR1, 0x00, 0xFF);
							while(spi_isbusy());
						}
					}
					//
				}
#endif //FAT_DEBUG
				write_fakefat();
			}
			next_sector:
			SectorNumber += wSectorSize;
			lba++;
			FakeLBA++;
    }
		P70 = 1;// LED on
}

void scsi_verify10()
{
}


