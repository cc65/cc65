
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 25.12.99

; char FindFTypes  (char *buffer, char fileType, char fileMax, char *Class);

	    .export _FindFTypes
	    .import popax, popa

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_FindFTypes:
	sta r10L
	stx r10H
	jsr popa
	sta r7H
	jsr popa
	sta r7L
	jsr popax
	sta r6L
	stx r6H
	jsr FindFTypes
	stx errno
	txa
	rts
