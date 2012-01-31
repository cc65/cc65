
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 21.12.1999, 2.1.2003

; char FindBAMBit (struct tr_se *TS);
; (might be called inUSE (if (!inUSE(block))))

	    .import gettrse
	    .import return0, return1
	    .export _FindBAMBit

	    .include "jumptab.inc"
	    .include "geossym.inc"
	
_FindBAMBit:
	jsr gettrse
	sta r6L
	stx r6H
	jsr FindBAMBit
	bne inUse
	jmp return0
inUse:	jmp return1
