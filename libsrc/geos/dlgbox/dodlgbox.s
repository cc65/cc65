
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 25.12.99

; char DoDlgBox         (char *myParamString);

	    .export _DoDlgBox

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"

_DoDlgBox:
	    sta r0L
	    stx r0H
	    jsr DoDlgBox
	    lda r0L
	    rts
