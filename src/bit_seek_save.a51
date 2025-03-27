;bit-seek read procedure
;R7 == 0: forward seek
;R7 == 1: reverse seek
;
;extern char bit_seek_read(char dir, char * param_ptr);

EXTRN CODE (_bit_seek_read)

DPS    DATA 0E3H
DPL1	DATA 0E4H
DPH1	DATA 0E5H
IAP_DAT DATA 0C2H
IAP_HI DATA 0C3H
IAP_LO DATA 0C4H
IAP_CMD DATA 0C5H
IAP_TR DATA 0C6H
IAP_CON DATA 0C7H
IAP_TPS DATA 0F5H
;TA     DATA 0AEH
;LED    DATA 0F8H

?PR?bit_seek_save SEGMENT CODE
	PUBLIC  _bit_seek_save

	RSEG    ?PR?bit_seek_save

_bit_seek_save:
; forward seek
	;
	PUSH DPS
	MOV A, R7
	MOV R6, A
	MOV R7, #00H
	MOV DPH, R2
	MOV DPL, R1
	MOV DPH1, R2
	MOV DPL1, R2
	;MOV DPTR, #param_N_ptr
	;LJMP return;debug
	JNZ seek_backward
;
seek_forward:
	;autoinc dual DPTR
	MOV DPS, #019H
	MOVX A, @DPTR
	ORL DPS, #020H
	;MOV DPTR, #param1_ptr
next_byte_fwd:
	CLR A
	MOVC A, @A+DPTR
	MOV B, A
	CLR A
	MOVC A, @A+DPTR
	;MOVX A, @DPTR;debug xram var
	JZ next_byte_fwd
	MOV C, B.7
next_bit_fwd:
	JB ACC.7, save_fwd
	INC R7
	RLC A
	SJMP next_bit_fwd
save_fwd:
	;
	MOV DPS, #0D8H
	PUSH ACC
	MOVX A, @DPTR
	POP ACC
	JNB ACC.6, buf_full;need erase block
	CLR ACC.7
shift_fwd:
	CJNE R7, #00H, check_fwd
	SJMP return
check_fwd:
	RRC A
	DEC R7
	SJMP shift_fwd
	;end forward seek
;
;reverse seek
	;
seek_backward:
	;autodec dual DPTR
	;INC DPTR;debug reverse pointer offset
	;MOV DPS, #058H
	MOV DPS, #0D9H
	MOVX A, @DPTR
	ORL DPS, #020H
	;MOV DPTR, #param2_ptr
next_byte_rev:
	CLR A
	MOVC A, @A+DPTR
	MOV B, A
	CLR A
	MOVC A, @A+DPTR
	;MOVX A, @DPTR;debug xram var
	JZ next_byte_rev
	MOV C, B.0
next_bit_rev:
	JB ACC.0, save_rev
	INC R7
	RRC A
	SJMP next_bit_rev
save_rev:
	;
	MOV DPS, #018H
	INC DPTR
	JNB ACC.1, buf_full;need erase block
	CLR ACC.0
shift_rev:
	CJNE R7, #00H, check_rev
	SJMP return
check_rev:
	RLC A
	DEC R7
	SJMP shift_rev
	;end reverse seek
;
return:
	;param_N save & return
	;
	MOV IAP_CON, #080H
	MOV IAP_TPS, #48D
	MOV IAP_DAT, A;#07FH
	;MOV IAP_HI, DPH
	MOV A, DPH
	ANL A, #01H
	MOV IAP_HI, A
	MOV IAP_LO, DPL
	;
	MOV IAP_CMD, #02H
	MOV IAP_TR, #05AH
	MOV IAP_TR, #0A5H
	NOP
	NOP
	NOP
	NOP
	MOV IAP_CON, #00H
	;MOV IAP_HI, #080H
	;MOV IAP_LO, #00H
	;MOV IAP_CMD, #00H
	;
	MOV A, R7
	ANL A, #01H
	MOV R7, A
	POP DPS
	RET; R7 return
;--------------------------- UGLY ---------------------
buf_full:
	;LJMP return;debug
	;MOV A, R6
	;CPL ACC.0
	MOV R7, #0FFH
	;PUSH DPS
	;LCALL _bit_seek_read
	RET
	END