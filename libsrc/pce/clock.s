;
; Ullrich von Bassewitz, 21.09.1998
;
; clock_t clock (void);
;

        .include "pcengine.inc"

        .export  	_clock
        .importzp       sreg
 
.proc	_clock

        ldy #0  	    	; Byte 3 is always zero
        sty sreg+1
        sty sreg

        ldx _tickcount+1
        lda _tickcount
        rts

.endproc

