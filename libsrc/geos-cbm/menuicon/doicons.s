
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 30.10.99

; void DoIcons         (struct icontab *myicons);

	    .export _DoIcons

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"

_DoIcons:
	    sta r0L
	    stx r0H
	    jmp DoIcons