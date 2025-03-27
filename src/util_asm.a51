;ASM Utils
;
; extern void *memset_xram(char xdata * addr, char dat, int len);
;
;ugly
;
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
	
?PR?memset_xram SEGMENT CODE
;?DT?memset_xram SEGMENT DATA

	PUBLIC  _memset_xram
	;PUBLIC  ?memset_xram?BYTE
		
	;RSEG    ?DT?memset_xram?BYTE
;?memset_xram?BYTE:
;arg?00:          DS  2

	RSEG    ?PR?memset_xram
	;USING 0

_memset_xram:
; memset util/xram fill
	USING 0
	PUSH DPS
	MOV DPH, R6
	MOV DPL, R7
	;MOV B, R2
	;MOV R6, B
	;MOV B, R3
	;MOV R7, B
	MOV DPS, #018H;use autoinc DPTR
	MOV A, R2
	JNZ inc_r2_not_need_memset_xram
	ORL A, R3
	JZ exit_memset_xram
	INC R2
	inc_r2_not_need_memset_xram:
	MOV A, R5
repeat:
	MOVX @DPTR, A
	DJNZ R3, repeat
	DJNZ R2, repeat
	exit_memset_xram:
	POP DPS
	RET; return addr (R6-R7)
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	Text labels copying from code to xdata
?PR?copy_labels SEGMENT CODE

	PUBLIC  _copy_labels

	RSEG    ?PR?copy_labels

_copy_labels:
	PUSH DPS
	MOV DPH, R4;code ptr
	MOV DPL, R5
	MOV DPH1, R6;xdata ptr
	MOV DPL1, R7
	;MOV DPS, #0x01
	;MOV DPTR, #0x1000
	;MOV DPS, #0x00
	;MOV DPTR, #0xF000
	;
	CLR A
	MOVC A, @A+DPTR
	INC DPTR
	MOV R6, A
	;
copy_table:
	MOV DPS, #0x00
	CLR A
	MOVC A, @A+DPTR
	INC DPTR
	JZ copy_label_skip; for \r\n
	MOV R7, A
copy_label:
	CLR A
	MOVC A, @A+DPTR
	INC DPTR
	MOV DPS, #0x01
	MOVX @DPTR, A
	INC DPTR
	MOV DPS, #0x00
	DJNZ R7, copy_label
	;
	CLR A
	MOVC A, @A+DPTR
	INC DPTR
	MOV DPS, #0x01
	JZ copy_label_skip
	MOV R7, A
fill_label:
	MOV A, #0x20
	MOVX @DPTR, A
	INC DPTR
	DJNZ R7, fill_label
	;
copy_label_skip:
	MOV DPS, #0x01
	MOV A, #0x0D
	MOVX @DPTR, A
	INC DPTR
	MOV A, #0x0A
	MOVX @DPTR, A
	INC DPTR
	DJNZ R6, copy_table
	MOV R6, DPH1
	MOV R7, DPL1
	POP DPS
	RET
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	Text params copying from code to xdata
?PR?copy_params SEGMENT CODE

	PUBLIC  _copy_params

	RSEG    ?PR?copy_params

_copy_params:				; R6-R7 XDATA PTR, R4-R5 CODE PTR, R3 SPACER SYMBOL
	PUSH DPS
	MOV DPH, R4;code ptr
	MOV DPL, R5
	MOV DPH1, R6;xdata ptr
	MOV DPL1, R7
	;
	MOV A, R4
	;JZ code_nullptr
	;MOV A, R5
	ORL A, R5
	JZ copy_code_nullptr
	;
	MOV DPS, #0x00
	CLR A
	MOVC A, @A+DPTR
	INC DPTR
	JZ copy_param_skip; for spacer symbol print
	;JZ code_nullptr2; for skip/spacer symbol print
	MOV R7, A
copy_param:
	CLR A
	MOVC A, @A+DPTR
	INC DPTR
	MOV DPS, #0x01
	MOVX @DPTR, A
	INC DPTR
	MOV DPS, #0x00
	DJNZ R7, copy_param
	LJMP copy_param_skip
	;
	copy_code_nullptr: ;;;; if code nullptr & spacer == \n --> return
	CJNE R3, #0x0A, check_space; "\n == NEW LINE CRLF else RETURN
	LJMP copy_new_line
	check_space:
	CJNE R3, #0x20, copy_return
	;MOV A, R3
	;JZ code_nullptr2
	;JNZ code_nullptr3
	;JNZ new_line_skip
copy_param_skip:
	MOV DPS, #0x01
	MOV A, R3; if code ptr & SPACER R3 == 0 --> NEW LINE CRLF
	JNZ copy_new_line_skip
	copy_new_line: ;;;; if code nullptr & spacer != \n --> NEW LINE CRLF
	MOV DPS, #0x01
	MOV A, #0x0D
	MOVX @DPTR, A
	INC DPTR
	MOV A, #0x0A
copy_new_line_skip:
	MOVX @DPTR, A
	INC DPTR
	;
	MOV R6, DPH1
	MOV R7, DPL1
copy_return:
	POP DPS
	RET
	

////
//WORD * fast_memcpy(char * out, char * inp, WORD len, char mode)
//{
//	//
//	return out;
//}

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	Fastest memcpy 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; SMALL MEMORY MODEL
;?PR?_fast_memcpy?UTIL	SEGMENT CODE
;?DT?_fast_memcpy?UTIL	SEGMENT DATA OVERLAYABLE  ;;; SMALL
		
	;PUBLIC	?_fast_memcpy?BYTE
	;PUBLIC	_fast_memcpy
		
	;RSEG 	?DT?_fast_memcpy?UTIL
;?_fast_memcpy?BYTE:
		;out_fast_memcpy:	DS	2
	;ORG	2
		;inp_fast_memcpy:	DS	3
	;ORG	5
		;len_fast_memcpy:	DS	2
	;ORG	7
		;mod_fast_memcpy:	DS	1
			
	;RSEG	?PR?_fast_memcpy?UTIL
;_fast_memcpy:
	;USING	0
	;PUSH DPS
	;;MOV DPS, #(DPTR0_AUTOEN | DPTR1_AUTOEN)
	;;MOV DPTR, #len_fast_memcpy
	;;MOVX A, @DPTR
	;MOV R4, len_fast_memcpy;A;;;;;; LEN HIGH
	;;MOVX A, @DPTR
	;MOV R5, len_fast_memcpy+01H;A;;;;;; LEN LOW
	;;MOVX A, @DPTR
	;;MOV R0, A;;;;;; MODE/FLOW CONTROL
	;;R3;;;;;;;;;;;; DPTR_0 IN MEMORY TYPE
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; COMMON PART
	;MOV DPH0, R2;;; DPTR_0 IN HIGH
	;MOV DPL0, R1;;; DPTR_0 IN LOW
	;MOV DPH1, R6;;; DPTR_1 OUT HIGH
	;MOV DPL1, R7;;; DPTR_1 OUT LOW
	
	;MOV DPS, #(DPTR0_AUTOEN | DPTR1_AUTOEN | DPTR_AUTOSEL | DPTR0_SEL)
	;MOV A, R3
	;JNZ dps_data_setup_fast_memcpy
	;SETB PSW.1;;; FOR DECREMENT DATA RAM POINTER
	;MOV DPS, #(DPTR0_AUTOEN | DPTR1_AUTOEN | DPTR1_SEL)
	;dps_data_setup_fast_memcpy:
	;;;;;;
	;;;;;;;;;;;;;;;;;; R5 7:6 -> 7: 0 OUT FWD, 1 OUT REV / 6: 0 IN FWD, 1 IN REV
	
	;RET
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	Fastest memcpy 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; LARGE MEMORY MODEL
?PR?_fast_memcpy?UTIL	SEGMENT CODE
?XD?_fast_memcpy?UTIL	SEGMENT XDATA OVERLAYABLE ;;; LARGE
		
	PUBLIC	?_fast_memcpy?BYTE
	PUBLIC	_fast_memcpy
		
	RSEG 	?XD?_fast_memcpy?UTIL
?_fast_memcpy?BYTE:
		;out_fast_memcpy:	DS	3
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		out_fast_memcpy:	DS	2
	ORG	2
		inp_fast_memcpy:	DS	3
	ORG	5
		len_fast_memcpy:	DS	2
	ORG	7
		mod_fast_memcpy:	DS	1
		
	RSEG	?PR?_fast_memcpy?UTIL
_fast_memcpy:
	USING	0
	PUSH DPS
	MOV DPS, #(DPTR0_AUTOEN | DPTR1_AUTOEN)
	MOV DPTR, #len_fast_memcpy
	MOVX A, @DPTR
	MOV R4, A;;;;;; LEN HIGH
	MOVX A, @DPTR
	MOV R5, A;;;;;; LEN LOW
	MOVX A, @DPTR
	MOV R0, A;;;;;; MODE/FLOW CONTROL
	;R3;;;;;;;;;;;; DPTR_0 IN MEMORY TYPE
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; COMMON PART
	MOV DPH0, R2;;; DPTR_0 IN HIGH
	MOV DPL0, R1;;; DPTR_0 IN LOW
	MOV DPH1, R6;;; DPTR_1 OUT HIGH
	MOV DPL1, R7;;; DPTR_1 OUT LOW
	
	MOV DPS, #(DPTR0_AUTOEN | DPTR1_AUTOEN | DPTR_AUTOSEL | DPTR0_SEL)
	MOV A, R3
	JNZ dps_data_setup_fast_memcpy
	;SETB PSW.1;;; FOR DECREMENT DATA RAM POINTER
	MOV DPS, #(DPTR0_AUTOEN | DPTR1_AUTOEN | DPTR1_SEL)
	dps_data_setup_fast_memcpy:
	;;;;;
	;;;;;;;;;;;;;;;;; R5 7:6 -> 7: 0 OUT FWD, 1 OUT REV / 6: 0 IN FWD, 1 IN REV
	MOV A, R4
	JNZ skip_inc_r4_fast_memcpy
	ORL A, R5
	JZ exit_fast_memcpy
	INC R4
	skip_inc_r4_fast_memcpy:
	MOV A, R0
	ORL DPS, A
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; MEMORY SELECT subroutine; Arg into ACC
	CLR C
	CLR F0
	MOV A, R3
	JNZ check_other_mem
	SETB F0
	SJMP fast_memcpy_loop
	check_other_mem:
	ADDC A, #02H
	
	;sjmp exit_fast_memcpy
	fast_memcpy_loop:
	JNB F0, memcpy_from_xcode_or_xdata
	MOV A, @R1
	INC R1
	SJMP memcpy_from_data
	memcpy_from_xcode_or_xdata:
	JNC memcpy_from_xdata
	memcpy_from_code:
	CLR A 
	MOVC A, @A+DPTR
	JC memcpy_from_xcode
	memcpy_from_xdata:
	MOVX A, @DPTR
	memcpy_from_data:
	memcpy_from_xcode:
	MOVX @DPTR, A
	DJNZ R5, fast_memcpy_loop
	DJNZ R4, fast_memcpy_loop
	exit_fast_memcpy:
	CLR F0
	MOV R7, DPL1
	MOV R6, DPH1
	
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; BIG FUCKING FUNC
	
	POP DPS
	RET

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	HEX TO ASCII CONVERT
?PR?_hex2str?UTIL    SEGMENT CODE 
?XD?_hex2str?UTIL    SEGMENT XDATA OVERLAYABLE 
	
	PUBLIC	?_hex2str?BYTE
	PUBLIC	_hex2str
		
	RSEG  ?XD?_hex2str?UTIL
?_hex2str?BYTE:
        out_hex2str:   DS   2
	ORG	2
        inp_hex2str:   DS   3
	ORG 5
        len_hex2str:   DS   1
			
	RSEG  ?PR?_hex2str?UTIL
_hex2str:
	USING	0
	
	PUSH DPS
	MOV DPTR, #len_hex2str
	MOVX A, @DPTR
	MOV R5, A
	MOV DPL1, R7;DPTR_1 OUT
	MOV DPH1, R6
	MOV DPL, R1;DPTR_0 IN
	MOV DPH, R2
	
	MOV DPS, #(DPTR0_AUTOEN | DPTR1_AUTOEN | DPTR0_SEL)
	;MOV A, R3
	;JNZ dps_data_setup_hex2str
	;;SETB PSW.1;;; FOR DECREMENT DATA RAM POINTER
	;MOV DPS, #(DPTR0_AUTOEN | DPTR1_AUTOEN | DPTR0_SEL)
	;dps_data_setup_hex2str:
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; START
	MOV A, R5
	JZ exit_hex2str_asm_sub
	;;
	MOV C, ACC.7
	MOV F0, C
	JNC goto_convert
	CLR ACC.7
	MOV R5, A
	;
	goto_convert:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; MEMORY SELECT subroutine; Arg into ACC
	MOV A, R3;;;;;;;;;;;;;;;;;;;;;;;; Ptr type: 00 - data, 01 - xdata, FE - pdata, FF - xcode
	JNZ ptr_not_in_data;;;;;;;;;;;;;; ptr to data  | 00
	MOV A, @R1; INDIRECT RAM DATA
	INC R1
	SJMP start_byte_convert
	ptr_not_in_data:
	CLR C
	ADDC A, #02H
	JNC get_xdata_byte;;;;;;;;;;;;;;; ptr to xdata | 01 / ptr to pdata | FE
	CLR A;;;;;;;;;;;;;;;;;;;;;;;;;;;; ptr to xcode | FF
	MOVC A, @A+DPTR; INDIRECT XCODE
	SJMP start_byte_convert
	;
	get_xdata_byte:
	MOVX A, @DPTR; INDIRECT RAM XDATA | PDATA
	start_byte_convert:
	CLR C
	MOV R7, A;PUSH ACC
	SWAP A
	JNB F0, no_trim;;;;;;;;;;;;;;;;;;;;;; check trim flag
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; trim_buffer/seek_null_nibbles:
	JZ skip_cycle
	ANL A, #0FH
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; trim_done
	CLR F0
	JZ trim_low
	no_trim:
	LCALL conv_to_ascii;;;;;;;;;;;;; High, Arg into ACC
	trim_low:
	MOV A, R7;POP ACC
	LCALL conv_to_ascii;;;;;;;;;;;;; Low, Arg into ACC
	skip_cycle:
	DJNZ R5, goto_convert;;;;;;;;;;; while loop
	SJMP exit_hex2str_asm_sub;;;;;;; conv_to_ascii_done:
	;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; convert nibble to ascii subroutine; Arg into ACC
	conv_to_ascii:
	INC DPS;;;;;;;;;;;;;;;;;;;;;;;;;; select DPTR_1
	ANL A, #0FH
	MOV R6, A;PUSH ACC
	SUBB A, #0AH
	MOV A, R6;POP ACC
	JC add_0x30
	ADD A, #037H
	SJMP nib_to_ascii_ret
	add_0x30:
	ADD A, #030H
	nib_to_ascii_ret:
	CLR C
	MOVX @DPTR, A
	INC DPS;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; select DPTR_0
	RET
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; END convert nibble to ascii; Arg into ACC
	exit_hex2str_asm_sub:
	MOV R7, DPL1
	MOV R6, DPH1
	POP DPS
	RET
	
;?PR?_hex2str?UTIL    SEGMENT CODE 
;?XD?_hex2str?UTIL    SEGMENT XDATA OVERLAYABLE 
  
	;PUBLIC  ?_hex2str?BYTE
	;PUBLIC  _hex2str

	;RSEG  ?XD?_hex2str?UTIL
;?_hex2str?BYTE:
		;out_hex2str:   DS   2
	;ORG	2
		;inp_hex2str:   DS   3
	;ORG  5
		;len_hex2str:   DS   1
	  
	;RSEG  ?PR?_hex2str?UTIL
;_hex2str:
	;USING  0

	;MOV DPTR, #len_hex2str
	;MOVX A, @DPTR
	;MOV R5, A
	;MOV DPL1, R7;DPTR_1 OUT
	;MOV DPH1, R6
	;MOV DPL, R1;DPTR_0 IN
	;MOV DPH, R2
	;PUSH DPS
	;MOV DPS, #(DPTR0_AUTOEN | DPTR1_AUTOEN)
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; START
	;MOV A, R5
	;JZ exit_hex2str_asm_sub
	;;;
	;MOV C, ACC.7
	;JNC goto_convert
	;CLR ACC.7
	;MOV R5, A
	;;
	;goto_convert:
	;MOV A, R3;;;;;;;;;;;;;;;;;;;;;;;; Ptr type: 00 - data, 01 - xdata, FE - pdata, FF - xdata
	;JZ ptr_to_data;;;;;;;;;;;;;;;;;;; ptr to data  | 00
	;JB ACC.6, ptr_to_xcode;;;;;;;;;;; ptr to xcode | FF
	;ACALL get_xdata_byte;;;;;;;;;;;;; ptr to xdata | 01 / ptr to pdata | FE
	;SJMP start_byte_convert
	;ptr_to_xcode:
	;ACALL get_xcode_byte;;;;;;;;;;;;; ptr to xcode | FF
	;SJMP start_byte_convert
	;ptr_to_data:
	;ACALL get_data_byte
	;SJMP start_byte_convert
	;;
	;start_byte_convert:
	;MOV R7, A;PUSH ACC
	;SWAP A
	;JNC no_trim;;;;;;;;;;;;;;;;;;;;;; check trim flag
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; trim_buffer/seek_null_nibbles:
	;JZ skip_cycle
	;ANL A, #0FH
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; trim_done
	;CLR C;F0
	;JZ trim_low
	;no_trim:
	;ACALL conv_to_ascii;;;;;;;;;;;;; High, Arg into ACC
	;trim_low:
	;MOV A, R7;POP ACC
	;ACALL conv_to_ascii;;;;;;;;;;;;; Low, Arg into ACC
	;skip_cycle:
	;DJNZ R5, goto_convert;;;;;;;;;;; while loop
	;SJMP exit_hex2str_asm_sub;;;;;;; conv_to_ascii_done:
	;;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; DATA ACCESS subroutine; Arg into ACC
	;get_data_byte:
	;MOV A, @R1; INDIRECT RAM DATA
	;RET
	;;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; XDATA PDATA ACCESS subroutine; Arg into ACC
	;get_xdata_byte:
	;MOVX A, @DPTR; INDIRECT RAM XDATA | PDATA
	;RET
	;;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; XCODE ACCESS subroutine; Arg into ACC
	;get_xcode_byte:
	;CLR A
	;MOVC A, @A+DPTR; INDIRECT XCODE
	;RET
	;;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; convert nibble to ascii subroutine; Arg into ACC
	;conv_to_ascii:
	;INC DPS;;;;;;;;;;;;;;;;;;;;;;;;;; select DPTR_1
	;ANL A, #0FH
	;MOV R6, A;PUSH ACC
	;SUBB A, #0AH
	;MOV A, R6;POP ACC
	;JC add_0x30
	;ADD A, #037H
	;SJMP nib_to_ascii_ret
	;add_0x30:
	;ADD A, #030H
	;nib_to_ascii_ret:
	;CLR C
	;MOVX @DPTR, A
	;INC DPS;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; select DPTR_0
	;RET
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; END convert nibble to ascii; Arg into ACC
	;exit_hex2str_asm_sub:
	;MOV R7, DPL1
	;MOV R6, DPH1
	;POP DPS
	;RET

	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; new func

	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	END
	