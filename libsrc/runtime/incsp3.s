;
; Ullrich von Bassewitz, 2003-05-04
;
; CC65 runtime: Increment the stackpointer by 3
;

       	.export		incsp3
        .importzp       sp

.proc	incsp3

        lda     sp
        clc
        adc     #3
        sta     sp
        bcs     @L1
        rts

@L1:    inc     sp+1
        rts

.endproc

