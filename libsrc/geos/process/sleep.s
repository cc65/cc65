
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 30.10.99

;
; void Sleep (int jiffies);
;

	    .export _Sleep

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"

_Sleep:
	    
	    sta r0L
	    stx r0H
	    jmp Sleep