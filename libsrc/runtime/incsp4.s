;
; Ullrich von Bassewitz, 2003-05-04
;
; CC65 runtime: Increment the stackpointer by 4
;

       	.export		incsp4
        .importzp       sp

.proc	incsp4

        lda     sp
        clc
        adc     #4
        sta     sp
        bcs     @L1
        rts

@L1:    inc     sp+1
        rts

.endproc

