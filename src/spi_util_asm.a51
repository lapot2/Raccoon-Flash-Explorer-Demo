DPS DATA 0E3H
DPL0	DATA 082H
DPH0	DATA 083H	
DPL1	DATA 0E4H
DPH1	DATA 0E5H
	
	
SPSTAT	DATA	0CDH
SPDAT	DATA	0CFH
	
SPI_SS	BIT	0C8H.4
	
DPTR1_AUTODEC	EQU		0x80
DPTR0_AUTODEC	EQU		0x40
DPTR_AUTOSEL	EQU		0x20
DPTR1_AUTOEN	EQU		0x10
DPTR0_AUTOEN	EQU		0x08
DPTR1_SEL		EQU		0x01
DPTR0_SEL		EQU		0x00

;EXTRN	CODE (_spi_send)
	
?PR?set_ext_addr SEGMENT CODE

	PUBLIC  _set_ext_addr

	RSEG    ?PR?set_ext_addr

;_set_ext_addr:
	;USING 0
	;CLR SPI_SS
	;LCALL _spi_send
	;MOV R7, AR5
	;LCALL _spi_send
	;SETB SPI_SS
	;RET
	
_set_ext_addr:
	USING 0
	CLR SPI_SS
	
	MOV SPSTAT, #0C0H
	MOV SPDAT, R7
	MOV A, SPSTAT
	JNB ACC.7, $-2
		
	;MOV R7, AR5
	
	MOV SPSTAT, #0C0H
	MOV SPDAT, R5
	MOV A, SPSTAT
	JNB ACC.7, $-2
	
	SETB SPI_SS
	RET


?PR?_spi_send SEGMENT CODE
	
	PUBLIC  _spi_send
	
	RSEG    ?PR?_spi_send
_spi_send:
	USING 0
	MOV SPSTAT, #0C0H
	MOV SPDAT, R7
	;
	;CLR SPI_SS;debug PY25Q64
	;
	MOV A, SPSTAT
	JNB ACC.7, $-2
	MOV R7, SPDAT
	RET
	
; BOOL spi_isbusy() small
?PR?spi_isbusy SEGMENT CODE
	
	PUBLIC  spi_isbusy
		
	RSEG  ?PR?spi_isbusy
spi_isbusy:
	USING	0
	CLR  	SPI_SS
;     spi_send(SPI_RDSR1); 
	MOV  	R7,#05H
	LCALL	_spi_send
	;MOV  	R7, #00H
	LCALL	_spi_send
	SETB 	SPI_SS
;     
; 		return (dat & 0x01);
	MOV  	A,R7
	RRC  	A
	RET
	
; BOOL spi_parity_check(BYTE id) small
?PR?_spi_parity_check SEGMENT CODE
	
	PUBLIC  _spi_parity_check
		
	RSEG  ?PR?_spi_parity_check
_spi_parity_check:
	USING	0
	MOV A, R7
	RLC A
	CPL C
	RR A
	MOV B, PSW
	RLC A
	XRL A, B
	RRC A
	RET
	
	END
		
;_spi_parity_check:
	;USING	0
	;MOV A, R7
	;RLC A
	;CPL C
	;RR A
	;MOV B, PSW
	;;ANL B, #0x01
	;RLC A
	;;ANL A, #0x01
	;XRL A, B
	;RRC A
	;RET
