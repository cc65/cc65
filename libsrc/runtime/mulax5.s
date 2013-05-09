;
; Ullrich von Bassewitz, 04.10.2001
;
; CC65 runtime: Multiply the primary register
;

        .export         mulax5
        .importzp       ptr1

.proc   mulax5

        sta     ptr1
        stx     ptr1+1
        asl     a
        rol     ptr1+1
        asl     a
        rol     ptr1+1
        clc
        adc     ptr1
        pha
        txa
        adc     ptr1+1
        tax
        pla
        rts

.endproc


