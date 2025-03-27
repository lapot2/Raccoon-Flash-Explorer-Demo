DPS DATA 0E3H
DPL0	DATA 082H
DPH0	DATA 083H	
DPL1	DATA 0E4H
DPH1	DATA 0E5H
	
DPTR1_AUTODEC	EQU		0x80
DPTR0_AUTODEC	EQU		0x40
DPTR_AUTOSEL	EQU		0x20
DPTR1_AUTOEN	EQU		0x10
DPTR0_AUTOEN	EQU		0x08
DPTR1_SEL		EQU		0x01
DPTR0_SEL		EQU		0x00
	
?PR?spi_id_remap SEGMENT CODE

	PUBLIC  _spi_id_remap

	RSEG    ?PR?spi_id_remap

_spi_id_remap:
	USING 0
	PUSH DPS
	MOV DPH, R4
	MOV DPL, R5
	MOV DPS, #(DPTR1_AUTOEN|DPTR0_AUTOEN);|DPTR_AUTOSEL)
	SJMP spi_id_remap_start
	
	spi_id_remap_seek:
	INC DPTR
	spi_id_remap_start:
	CLR A
	MOVC A, @A+DPTR
	JZ spi_id_remap_not_found
	;
	CJNE A, AR7, spi_id_remap_seek
	;
	CLR A
	MOVC A, @A+DPTR
	spi_id_remap_not_found:
	MOV R7, A
	POP DPS
	RET
	
	END