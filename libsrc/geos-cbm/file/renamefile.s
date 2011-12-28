
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 25.12.1999, 2.1.2003

; char RenameFile  (char *old, char *new);

	    .export _RenameFile
	    .import popax, setoserror

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"

_RenameFile:
	sta r0L
	stx r0H
	jsr popax
	sta r6L
	stx r6H
	jsr RenameFile
	jmp setoserror
