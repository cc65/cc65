;
; Ullrich von Bassewitz, 04.10.2001
;
; CC65 runtime: Multiply the primary register by 3
;

        .export         mulax3
        .importzp       ptr1
                
.proc   mulax3

        sta     ptr1
        stx     ptr1+1
        asl     a
        rol     ptr1+1
        clc
        adc     ptr1
        tay
        txa
        adc     ptr1+1
        tax
        tya
        rts

.endproc


