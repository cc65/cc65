;
; Ullrich von Bassewitz, 2003-05-04
;
; CC65 runtime: Increment the stackpointer by 8
;

       	.export		incsp8
        .importzp       sp

.proc	incsp8

        lda     sp
        clc
        adc     #8
        sta     sp
        bcs     @L1
        rts

@L1:    inc     sp+1
        rts

.endproc

                  
