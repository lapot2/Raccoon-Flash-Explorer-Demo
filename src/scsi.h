#ifndef __SCSI_H__
#define __SCSI_H__

#ifndef NULL
#define NULL                    ((void *)0)
#endif

#define CBW_SIGNATURE           0x55534243
#define CSW_SIGNATURE           0x55534253

//		SCSI Status Codes
#define SCSI_PASSED 		    		0x00
#define SCSI_FAILED 		   			0x01
#define SCSI_PHASE_ERROR 	  		0x02

//		SCSI Status Codes
#define SCSI_PASSED									0x00
#define SCSI_CHECK_CONDITION				0x02
#define SCSI_CONDITION_MEET					0x04
#define SCSI_BUSY										0x08
#define SCSI_RESERVATION_CONFLICT		0x18
#define SCSI_TASK_SET_FULL					0x28
#define SCSI_ACA_ACTIVE							0x30
#define SCSI_TASK_ABORTED						0x40

// 		SCSI OPcodes
#define SCSI_TESTUNITREADY      0x00
#define SCCI_STARTSTOPUNIT      0x1b
#define SCSI_REQUESTSENSE       0x03
#define SCSI_INQUIRY            0x12
#define SCSI_MODESENSE6         0x1a
#define SCSI_MODESENSE10        0x5a
#define SCSI_MEDIAREMOVAL       0x1e
#define SCSI_FORMATCAPACITY     0x23
#define SCSI_READCAPACITY10     0x25
#define SCSI_READ10             0x28
#define SCSI_WRITE10            0x2a
#define SCSI_VERIFY10           0x2f

//#define SCSI_SYNCHRONIZE_CACHE	0x35

//	SCSI Sense Keys
#define SENSE_NOSENSE           0x00
#define SENSE_RECOVEREDERROR    0x01
#define SENSE_NOTREADY          0x02
#define SENSE_MEDIUMERROR       0x03
#define SENSE_HARDWAREERROR     0x04
#define SENSE_ILLEGALREQUEST    0x05
#define SENSE_UNITATTENTION     0x06
#define SENSE_DATAPROTECT       0x07
#define SENSE_BLANKCHECK        0x08
#define SENSE_VENDORSPECIFIC    0x09
#define SENSE_COPYABORTED       0x0a
#define SENSE_ABORTE_COMMAND    0x0b
#define SENSE_VOLUMEOVERFLOW    0x0d
#define SENSE_MISCOMPARE        0x0e

#define ASC_INVALIDCOMMAND      0x20
#define ASC_MEDIUMNOTPRESENT    0x3a

#define SIZEOF_CBW							31
typedef struct
{ 
    DWORD   dCBWSignature; 
    DWORD   dCBWTag; 
    DWORD   dCBWDataLength; 
    BYTE    bmCBWFlags; 
    BYTE    bCBWLUN; 
    BYTE    bCBWCBLength; 
    BYTE    bScsiOpcode;
    BYTE    ScsiParam[15]; 
}CBW; // sizeof(31);
 
#define SIZEOF_CSW							13
typedef struct
{
	DWORD   dCSWSignature; 
	DWORD   dCSWTag; 
	DWORD   dCSWDataResidue; 
	BYTE    bCSWStatus; 
}CSW; 

void scsi_process();

void scsi_in(BYTE *pData, int nSize);
void scsi_out(BYTE *pData, int nSize);

BOOL scsi_test_unit_ready();
void scsi_start_stop_unit();
void scsi_media_removal();
void scsi_inquiry();
void scsi_read_capacity10();
void scsi_format_capacity();
void scsi_request_sense();
void scsi_mode_sense6();
void scsi_mode_sense10();
void scsi_read10();
void scsi_write10();
void scsi_verify10();


extern DWORD data residue;

extern char OS_detect;
extern BOOL scsi_ready;
extern BOOL verify_err;
extern DWORD data verify_err_addr;

extern DWORD WrittenSectCount;

extern BYTE xdata scsibuf[4096];
extern BYTE xdata str_tmp[16];

#endif
