
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 29.10.99

; char TestPoint	(struct pixel *mypixel);

	    .import PointRegs
	    .export _TestPoint

	    .include "../inc/jumptab.inc"
	    
_TestPoint:
	    jsr PointRegs
	    jsr TestPoint
	    bcc goFalse
	    lda #$ff
	    rts
goFalse:    lda #0
	    rts
