	IMPORT 	OSTCBCur
	IMPORT	OSTCBNext
	
	EXPORT	OS_ENTER_CRITICAL
	EXPORT 	OS_EXIT_CRITICAL
	EXPORT	OSStart
	EXPORT	PendSV_Handler
	EXPORT 	OSCtxSw
	
NVIC_INT_CTRL	EQU	0xE000ED04	;Address of NVIC Interruptions Control Register
NVIC_PENDSVSET	EQU	0x10000000	;Enable PendSV
NVIC_SYSPRI14   EQU     0xE000ED22                              ; System priority register (priority 14).
NVIC_PENDSV_PRI EQU           0xFF                              ; PendSV priority value (lowest).
	
	PRESERVE8 

	AREA    |.text|, CODE, READONLY
	THUMB 

;/******************OS_ENTER_CRITICAL************/
OS_ENTER_CRITICAL
	CPSID	I	; Enable interruptions
	BX	LR	; Return

;/******************OS_EXIT_CRITICAL************/
OS_EXIT_CRITICAL
	CPSIE	I	; Disable interruptions
	BX	LR 	; Return

;/******************OSStart************/
OSStart
	; disable interruptions
	CPSID	I
	; initialize PendSV
	LDR     R0, =NVIC_SYSPRI14                                  ; Set the PendSV exception priority
	LDR     R1, =NVIC_PENDSV_PRI
	STRB    R1, [R0]
	
	; initialize PSP as 0
	MOV	R4, #0	
	MSR	PSP, R4	
	
	; trigger PendSV
	LDR	R4, =NVIC_INT_CTRL
	LDR	R5, =NVIC_PENDSVSET
	STR	R5, [R4]
	
	; enable interruptions
	CPSIE	I

; should never get here
OSStartHang
	B	OSStartHang

;/******************PendSV_Handler************/
PendSV_Handler
	CPSID	I
	; judge if using PSP
	MRS 	R0, PSP
	CBZ 	R0, PendSV_Handler_NoSave
	
	; judge PSR, PC, LR, R12
	; 	R12, R3, R2, R1
	;SUB 	R0, R0, #0x20
	;
	;; store R11 
	;STR 	R11, [R0]
	;SUB 	R0, R0, #0x4
	;; store R10 
	;STR 	R10, [R0]
	;SUB 	R0, R0, #0x4
	;; store R9
	;STR 	R9, [R0]
	;SUB 	R0, R0, #0x4
	;; store R8 
	;STR 	R8 , [R0]
	;SUB 	R0, R0, #0x4
	;; store R7 
	;STR 	R7 , [R0]
	;SUB 	R0, R0, #0x4
	;; store R6 
	;STR 	R6 , [R0]
	;SUB 	R0, R0, #0x4
	;; store R5 
	;STR 	R5 , [R0]
	;SUB 	R0, R0, #0x4
	;; store R4 
	;STR 	R4 , [R0]
	;SUB 	R0, R0, #0x4
	;ADD 	R0, R0, #0x20
	SUB 	R0, R0, #0x20
	STM 	R0, {R4-R11}
	
	LDR 	R1, =OSTCBCur
	LDR 	R1, [R1]
	STR 	R0, [R1]

PendSV_Handler_NoSave
	; OSTCBCur->OSTCBStkPtr = OSTCBNext->OSTCBStkPtr;
	LDR 	R0, =OSTCBCur 
	LDR 	R1, =OSTCBNext
	LDR 	R2, [R1]
	STR 	R2, [R0]
	
	LDR 	R0, [R2]
	; LDM 	R0, {R4-R11}
	; load R4 
	LDR 	R4, [R0]
	ADD 	R0, R0, #0x4
	; load R5 
	LDR 	R5, [R0]
	ADD 	R0, R0, #0x4
	; load R6
	LDR 	R6, [R0]
	ADD 	R0, R0, #0x4
	; load R7 
	LDR 	R7 , [R0]
	ADD 	R0, R0, #0x4
	; load R8 
	LDR 	R8 , [R0]
	ADD 	R0, R0, #0x4
	; load R9 
	LDR 	R9 , [R0]
	ADD 	R0, R0, #0x4
	; load R10 
	LDR 	R10 , [R0]
	ADD 	R0, R0, #0x4
	; load R11 
	LDR 	R11 , [R0]
	ADD 	R0, R0, #0x4
	
	MSR 	PSP, R0
	ORR 	LR, LR, #0x04
	CPSIE 	I
	BX	LR

OSCtxSw
	PUSH	{R4, R5}
	LDR 	R4, =NVIC_INT_CTRL
	LDR 	R5, =NVIC_PENDSVSET
	STR 	R5, [R4]
	POP 	{R4, R5}
	BX 	LR
	
	align 4
	end
