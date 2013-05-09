;
; Piotr Fusik, 24.10.2003
; originally by Ullrich von Bassewitz
;
; CC65 runtime: Multiply the primary register by 10
;

        .export         mulax10
        .importzp       ptr1

.proc   mulax10

        sta     ptr1
        stx     ptr1+1
        asl     a
        rol     ptr1+1
        asl     a
        rol     ptr1+1
        clc
        adc     ptr1
        sta     ptr1
        txa
        adc     ptr1+1
        asl     ptr1
        rol     a
        tax
        lda     ptr1
        rts

.endproc


