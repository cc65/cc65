;
; Piotr Fusik, 24.10.2003
; originally by Ullrich von Bassewitz
;
; CC65 runtime: Multiply the primary register by 9
;
; Don't touch the Y-register here, the optimizer relies on it!

        .export         mulax9
        .importzp       ptr1

.proc   mulax9

        sta     ptr1
        stx     ptr1+1
        asl     a
        rol     ptr1+1                  ; * 2
        asl     a
        rol     ptr1+1                  ; * 4
        asl     a
        rol     ptr1+1                  ; * 8
        clc
        adc     ptr1                    ; * (8+1)
        pha
        txa
        adc     ptr1+1
        tax
        pla
        rts

.endproc


