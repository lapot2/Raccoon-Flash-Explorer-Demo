#ifndef __SCSI_RW_PARSER_H__
#define __SCSI_RW_PARSER_H__


//************************ READ ***************************
void read_fakefat();
void read_mem(DWORD addr, int nSize) small;
void verify_mem(DWORD addr, int nSize) small;
void read_regs();
void read_file1();
void read_file2();
void read_file3();
//*********************** WRITE ***************************

void write_fakefat();
void write_mem(DWORD addr, int nSize) small;
void write_regs();
void write_file1();
void write_file2();
void write_file3();

extern BOOL verify_enabled;

#endif //__SCSI_RW_PARSER_H__