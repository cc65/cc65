;
; Ullrich von Bassewitz, 2003-05-04
;
; CC65 runtime: Increment the stackpointer by 6
;

       	.export		incsp6
        .importzp       sp

.proc	incsp6

        lda     sp
        clc
        adc     #6
        sta     sp
        bcs     @L1
        rts

@L1:    inc     sp+1
        rts

.endproc

              
