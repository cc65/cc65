
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 22.12.99

	    .import popax
	    .importzp ptr3, ptr4
	    .export DoubleSPop

	    .include "../inc/geossym.inc"

DoubleSPop:
	    sta ptr4
	    stx ptr4+1
	    jsr popax
	    sta ptr3
	    stx ptr3+1
	    lda #ptr4
	    ldx #ptr3
	    rts
