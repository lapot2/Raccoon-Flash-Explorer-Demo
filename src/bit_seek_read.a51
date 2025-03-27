;bit-seek read procedure
;R7 == 0: forward seek
;R7 == 1: reverse seek
;
;extern char bit_seek_read(char dir, char * param_ptr);

DPS DATA 0E3H
;TA  DATA 0AEH
;LED DATA 0F8H

?PR?bit_seek_read SEGMENT CODE
	PUBLIC  _bit_seek_read

	RSEG    ?PR?bit_seek_read

_bit_seek_read:
; forward seek
	;
	PUSH DPS
	MOV A, R7
	MOV R7, #00H
	MOV DPH, R2
	MOV DPL, R1
	JNZ seek_backward
seek_forward:
	;autoinc DPTR
	MOV DPS, #018H
	;MOV DPTR, #param1_ptr
next_byte_fwd:
	CLR A
	MOVC A, @A+DPTR
	;MOVX A, @DPTR;debug xram var
	JZ next_byte_fwd
next_bit_fwd:
	JB ACC.7, return
	INC R7
	RL A
	SJMP next_bit_fwd
	;end forward seek
;
;reverse seek
	;
seek_backward:
	;autodec DPTR
	;INC DPTR;debug reverse pointer offset
	MOV DPS, #058H
	;MOV DPTR, #param2_ptr
next_byte_rev:
	CLR A
	MOVC A, @A+DPTR
	;MOVX A, @DPTR;debug xram var
	JZ next_byte_rev
next_bit_rev:
	JB ACC.0, return
	INC R7
	RR A
	SJMP next_bit_rev
return:
	MOV A, R7
	ANL A, #01H
	MOV R7, A
	POP DPS
	RET; R7 return
	END