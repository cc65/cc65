;
; Ullrich von Bassewitz, 2009-08-31
;
; CC65 runtime: Load effective address with offset in A/X relative to SP
;

    	.export		leaaxsp
     	.importzp	sp

.proc	leaaxsp
             
        clc
        adc     sp
        pha
        txa
        adc     sp+1
        tax
        pla
        rts

.endproc


