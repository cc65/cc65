;
; Written by Groepaz/Hitmen <groepaz@gmx.net>
; Cleanup by Ullrich von Bassewitz <uz@cc65.org>
;
; clock_t clock (void);
;

        .include "nes.inc"

      	.export	       	_clock
        .importzp	sreg


.proc	_clock

        ldy    	#0  	    	; High word is always zero
       	sty     sreg+1
       	sty     sreg
        ldx     tickcount+1    ; ## Problem: Cannot disable ints here
        lda     tickcount
       	rts

.endproc

