
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 25.12.99, 30.7.2000

; char FindFTypes  (char *buffer, char fileType, char fileMax, char *Class);

	    .export _FindFTypes
	    .import popax, popa, __oserror

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_FindFTypes:
	sta r10L
	stx r10H
	jsr popa
	sta r7H
	sta tmpFileMax
	jsr popa
	sta r7L
	jsr popax
	sta r6L
	stx r6H
	jsr FindFTypes
	stx __oserror
; return (fileMax - r7H)
	lda tmpFileMax
	sec
	sbc r7H
	rts

tmpFileMax:	.byte 0
