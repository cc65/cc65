;
; Piotr Fusik, 24.10.2003
; originally by Ullrich von Bassewitz
;
; CC65 runtime: Multiply the primary register by 7
;
; Don't touch the Y-register here, the optimizer relies on it!

        .export         mulax7
        .importzp       ptr1

.proc   mulax7

        sta     ptr1
        stx     ptr1+1
        asl     a
        rol     ptr1+1                  ; * 2
        asl     a
        rol     ptr1+1                  ; * 4
        asl     a
        rol     ptr1+1                  ; * 8
        sec
        sbc     ptr1
        pha
        txa
        eor     #$ff
        adc     ptr1+1                  ; * (8 - 1)
        tax
        pla
        rts

.endproc


