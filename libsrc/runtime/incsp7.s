;
; Ullrich von Bassewitz, 2003-05-04
;
; CC65 runtime: Increment the stackpointer by 7
;

       	.export		incsp7
        .importzp       sp

.proc	incsp7

        lda     sp
        clc
        adc     #7
        sta     sp
        bcs     @L1
        rts

@L1:    inc     sp+1
        rts

.endproc

                  
