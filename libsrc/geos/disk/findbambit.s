
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 21.12.99

; char FindBAMBit (struct tr_se *TS);
; (might be called inUSE (if (!inUSE(block))))

	    .import gettrse
	    .export _FindBAMBit

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_FindBAMBit:
	jsr gettrse
	sta r6L
	stx r6H
	jsr FindBAMBit
	bne inUse
	lda #0
	rts
inUse:	lda #$ff 
	rts
