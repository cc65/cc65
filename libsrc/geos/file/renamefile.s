
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 25.12.99

; char RenameFile  (char *source, char *target);

	    .export _RenameFile
	    .import popax, __oserror

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_RenameFile:
	sta r0L
	stx r0H
	jsr popax
	sta r6L
	stx r6H
	jsr RenameFile
	stx __oserror
	txa
	rts
