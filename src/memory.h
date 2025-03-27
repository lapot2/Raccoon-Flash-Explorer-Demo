#ifndef __MEMORY_H__
#define __MEMORY_H__


extern DWORD dwTotalMemSize;
extern DWORD dwTotalSectorCount;

extern BYTE ClusterSize;

extern DWORD data dwMemorySize;
extern WORD wSectorSize;
extern DWORD dwSectorCount;//extern DWORD dwSectorCount;

extern BYTE xdata ftable[2048];

//extern uchar xdata memory_type[3];
extern uchar xdata memory_name[11];


extern BOOL spi;
extern BOOL i2c;

extern BOOL ic_erase_flag;

extern BOOL fFlashOK;

extern BYTE code FAT_MBR_Data;
extern BYTE code FAT_PBR_Data;
extern BYTE code rawRoot_entry0;
extern DWORD flash_start_sector_offset;

WORD entry_load(BYTE * SecBuff, WORD EntryOffset);
void fat_make(BYTE * pdat);
void FakeFAT_init();
void FakeFat_readsector(DWORD addr, BYTE *pdat);


//*********************** FakeFS Fat16
/*
typedef struct {
	char PartState;
	char PartStartHead;
	char PartStartCylSec[2];
	char PartType;
	char PartEndHead;
	char PartEndCylHea[2];
	char LehgthToPart[4];
	char PartCapacity[4];
}PART_ENTRY;//MBR Partition entry

typedef struct {
	char ExecCode[446];
	PART_ENTRY PartEntry1;
	PART_ENTRY PartEntry2;
	PART_ENTRY PartEntry3;
	PART_ENTRY PartEntry4;
	char ExecMarker[2];
}FAT16MBR;//512 bytes | MBR FAT16 sector
*/
/*
typedef struct {
	char JmpCode[3];//0x00
	char OEMName[8];//0x03
	WORD SectorSize;//[2];//0x0b
	char ClustSize;//0x0d | Sectors per cluster
	WORD ReservedSect;//[2];//0x0e
	char NumCopiesFAT;//0x10 | Num copies of FAT
	WORD MaxRoot;//[2];//0x11 | Max entries in root
	WORD NumSectSmallParts;//[2];//0x13
	char MediaDescriptor;//0x15
	WORD SectPerFAT;//[2];//0x16 | Num sectors per FAT table
	WORD SectPerTrack;//[2];//0x18
	WORD NumHeads;//[2];//0x1a
	DWORD NumHiddenSect;//[4];//0x1c
	DWORD NumSect;//[4];//0x20
	WORD LunNumDrive;//[2];//0x24
	char ExtdSig;//0x26
	DWORD SnPart;//[4];//0x27
	char VolName[11];//0x2b
	char FATName[8];//0x36
	char ExecCode[448];//0x3e
	WORD Execmarker;//[2];//0x1fe
}FAT16PBR;//512 bytes | PBR FAT16 sector
*/

typedef union {
	uchar raw_PBR[512];
	struct {
		uchar JmpCode[3];//0x00
		uchar OEMName[8];//0x03
		WORD SectorSize;//[2];//0x0b
		uchar ClusterSize;//0x0d | Sectors per cluster
		WORD ReservedSect;//[2];//0x0e
		uchar NumCopiesFAT;//0x10 | Num copies of FAT | 
		WORD MaxRoot;//[2];//0x11 | Max entries in root | 512 | 512*32/4096 = 4 sectors for root dir
		WORD NumSectSmallParts;//[2];//0x13 Num sectors in disc
		uchar MediaDescriptor;//0x15
		WORD SectPerFAT;//[2];//0x16 | Num sectors per FAT table
		WORD SectPerTrack;//[2];//0x18
		WORD NumHeads;//[2];//0x1a
		DWORD NumHiddenSect;//[4];//0x1c
		DWORD NumSect;//[4];//0x20
		WORD LunNumDrive;//[2];//0x24
		uchar ExtdSig;//0x26
		DWORD SnPart;//[4];//0x27
		uchar VolName[11];//0x2b
		uchar FATName[8];//0x36
		uchar ExecCode[448];//0x3e
		WORD Execmarker;//[2];//0x1fe
#ifdef __KEIL__
	}PBR;
#else
	};
#endif // __KEIL__
}FAT16PBR;//512 bytes | PBR FAT16 sector


/*typedef union {
	uchar raw_PBR[512];
	struct {
		uchar JmpCode[3];//0x00
		uchar OEMName[8];//0x03
		uchar SectorSize[2];//0x0b
		uchar ClusterSize;//0x0d | Sectors per cluster
		uchar ReservedSect[2];//0x0e
		uchar NumCopiesFAT;//0x10 | Num copies of FAT | 
		uchar MaxRoot[2];//0x11 | Max entries in root | 512 | 512*32/4096 = 4 sectors for root dir
		uchar NumSectSmallParts[2];//0x13 Num sectors in disc
		uchar MediaDescriptor;//0x15
		uchar SectPerFAT[2];//0x16 | Num sectors per FAT table
		uchar SectPerTrack[2];//0x18
		uchar NumHeads[2];//0x1a
		uchar NumHiddenSect[4];//0x1c
		uchar NumSect[4];//0x20
		uchar LunNumDrive[2];//0x24
		uchar ExtdSig;//0x26
		uchar SnPart[4];//0x27
		uchar VolName[11];//0x2b
		uchar FATName[8];//0x36
		uchar ExecCode[448];//0x3e
		uchar Execmarker[2];//0x1fe
#ifdef __KEIL__
	}PBR;
#else
	};
#endif // __KEIL__
}FAT16PBR;//512 bytes | PBR FAT16 sector
*/

typedef union{
	char raw_LFN[32];
	struct{
		char LDIR_Ord;//0x00 LFN part Number
		WORD LDIR_Name1[5];//0x01|0xYY << 8
		char LDIR_Attr;//0x0b|0x0f
		char LDIR_Type;//0x0c|0x00
		char LDIR_Chksum;//0x0d
		WORD LDIR_Name2[6];//0x0e
		WORD LDIR_FstClusLO;//0x1c|0x0000
		WORD LDIR_Name3;//0x1e | Null for Dir
	}LFN;
}FS_LFN_ENTRY;
			
typedef union{
	uchar raw_SFN[32];
	struct{
		uchar DosFileName[8];//0x00
		uchar DosFileExt[3];//0x08
		uchar FileAttrib;//0x0b // READ_ONLY=0x01 HIDDEN=0x02 SYSTEM=0x04 VOLUME_ID=0x08 DIRECTORY=0x10 ARCHIVE=0x20
		uchar NTCaseInfo;//0x0c
		uchar CreateTimeMs;//0x0d 10ms Units
		uchar CreateTimeHMS[2];//0x0e
		uchar CreateDate[2];//0x10
		uchar LastAccessDate[2];//0x12
		uchar StartClusterHigh[2];//0x14 FAT32 Only
		uchar LastModTime[2];//0x16
		uchar LastModDate[2];//0x18
		WORD StartClusterLow;//[2];//0x1a
		DWORD FileSize;//[4];//0x1c | in bytes | Null for Dir
	}SFN;
}FS_ROOT_ENTRY;

//#define READONLY 0x01
//#define HIDDEN   0x02
//#define SYSTEM   0x04
//#define VOLUME   0x08
//#define DIR      0x10
//#define ACHIEVE  0x20

//#define READONLY 1<<0
//#define HIDDEN   1<<1
//#define SYSTEM   1<<2
//#define VOLUME   1<<3
//#define DIR      1<<4
//#define ACHIEVE  1<<5

/*
MS-DOS records file dates and times as packed 16-bit values. An MS-DOS date has the following format.

Bits		Contents
0–4		| Day of the month (1–31).
5–8		| Month (1 = January, 2 = February, and so on).
9–15	| Year offset from 1980 (add 1980 to get the actual year).
 
An MS-DOS time has the following format.

Bits		Contents
0–4		| Second divided by 2.
5–10	| Minute (0–59).
11–15	| Hour (0– 23 on a 24-hour clock).
 
The DosDateTimeToVariantTime function will accept invalid dates and try to fix them when resolving to a VARIANT time.
	For example, an invalid date such as 2/29/2001 will resolve to 3/1/2001.
Only days are fixed, so invalid month values result in an error being returned. Days are checked to be between 1 and 31. 
	Negative days and days greater than 31 results in an error. 
A day less than 31 but greater than the maximum day in that month has the day promoted to the appropriate day of the next month. 
	A day equal to zero resolves as the last day of the previous month. 
	For example, an invalid date such as 2/0/2001 will resolve to 1/31/2001.*/
	
	/*
									File attribute
		Flag														Meaning
ATTR_READ_ONLY	| Read-only File. Any changes to the file or delete should be rejected.
ATTR_HIDDEN			| Normal directoly listing should not show this file. (system dependent)
ATTR_SYSTEM			| Indicates this is an system file. (system dependent)
ATTR_DIRECTORY	| Indicates this is a container of a directory.
ATTR_ARCHIVE		| This is for backup utilities. Set by FAT driver on new creation, modification or renaming to the file is made. 
								| The backup utilities able to easily find the file to be backed up and it clears the attribute after the back up process.
ATTR_VOLUME_ID	| An entry with this attribute has the volume label of the volume. Only one entry can be exist in the root directory. 
								| DIR_FstClusHI, DIR_FstClusLO and DIR_FileSize field must be always zero. Some system may set ATTR_ARCHIVE, but it has no meaning.
ATTR_LONG_NAME	| This combination of attributes indicates the entry is a part of long file name. Details are described below.
*/

#endif

