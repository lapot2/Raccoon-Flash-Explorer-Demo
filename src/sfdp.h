
typedef struct{
	DWORD header;//BYTE header[4];
	BYTE minor_ver;
	BYTE major_ver;
	BYTE num_headers;
	BYTE protocol;
}sfdp_base_t;

typedef struct{
	// SFDP Param Table Header | 8 bytes
	BYTE pt_id_lsb;
	BYTE pt_minor_ver;
	BYTE pt_major_ver;
	BYTE pt_table_len;
	
	BYTE pt_a0;
	BYTE pt_a1;
	BYTE pt_a2;
	BYTE pt_id_msb;
}sfdp_ptheader_t;

typedef struct{
	BYTE rw_params[4];/*Uniform 4KB Sectors, Write Buffer Size, \
	Volatile Status Register, \
	Fast Read Support (1S-1S-2S) (1S-2S-2S) (1S-4S-4S)(1S-1S-4S), \
	Number of Address Bytes, DTR Support*/
	DWORD mem_size;//Memory Density
	DWORD fastrd_params_1;//Fast Read (1S-4S-4S) (1S-1S-4S): Wait States, Mode Bit Clocks, Instruction
	DWORD fastrd_params_2;//Fast Read (1S-1S-2S) (1S-2S-2S): Wait States, Mode Bit Clocks, Instruction
	DWORD fastrd_params_3;//Fast Read (2S-2S-2S) (4S-4S-4S) Support
	DWORD fastrd_params_4;//Fast Read (2S-2S-2S): Wait States, Mode Bit Clocks, Instruction
	DWORD fastrd_params_5;//Fast Read (4S-4S-4S): Wait States, Mode Bit Clocks, Instruction
	DWORD erase_params_1;//Erase Type 1 and 2 Size and Instruction
	DWORD erase_params_2;//Erase Type 3 and 4 Size and Instruction
	DWORD erase_params_3;//Erase Type (1:4) Typical Erase Times and Multiplier Used To Derive Max Erase Times
	DWORD erase_params_4;//Chip Erase Typical Time, Byte Program and Page Program Typical Times, Page Size
	DWORD erase_params_5;//Erase/Program Suspend/Resume Support, Intervals, Latency, Keep Out Area Size
	DWORD erase_params_6;//Program/Erase Suspend/Resume Instructions
	DWORD sr_poll;//Deep Powerdown and Status Register Polling Device Busy
	DWORD quad_params;/*Hold and WP Disable Function, \
	Quad Enable Requirements, 4S-4S-4S Mode Enable/DisableSequences, \
	0-4-4 Entry/Exit Methods and Support*/
	DWORD sr_params;/*32-bit Address Entry/Exit Methods and Support, \
	Soft Reset and Rescue Sequences, \
	Volatile andNonvolatile Status Register Support*/
	DWORD fastrd_params_6;//Fast Read (1S-8S-8S) (1S-1S-8S): Wait States, Mode Bit Clocks, Instruction
	DWORD octal_params_1;//Octal commands, Byte order, Data strobe, JEDEC SPI Protocol Reset
	DWORD octal_params_2;/*Octal Enable Requirements, \
	8D-8D-8D Mode Enable/Disable Sequences, 0-8-8 Entry/Exit Methodsand Support*/
	DWORD max_speed;//Maximum operating speeds
	DWORD fastrd_params_7;//Fast Read (1S-1D-1D), (1S-2D-2D), (1S-4D-4D), and (4S-4D-4D) Support
	DWORD fastrd_params_8;//Fast Read (1S-1D-1D) and (1S-2D-2D) Wait States, Mode Bit Clocks, and Instruction
	DWORD fastrd_params_9;//Fast Read (1S-4D-4D) and (4S-4D-4D) Wait States, Mode Bit Clocks, and Instruction
	// end of basic params table
}sfdp_bfptable_t;

BYTE sfdp_read(BYTE * buf, WORD len);

extern BYTE sfdp_header[256];
extern DWORD sfdp_mem_size;