;
; Ullrich von Bassewitz, 2003-05-04
;
; CC65 runtime: Increment the stackpointer by 5
;

       	.export		incsp5
        .importzp       sp

.proc	incsp5

        lda     sp
        clc
        adc     #5
        sta     sp
        bcs     @L1
        rts

@L1:    inc     sp+1
        rts

.endproc

                  
