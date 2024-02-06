;
; Ullrich von Bassewitz, 25.07.2001
;
; CC65 runtime: Scale the 32 bit primary register by 16
;

        .export         asleax4, shleax4
        .importzp       sreg, tmp1

asleax4:
shleax4:
        stx     tmp1
        ldx     #4
@L1:    asl     a
        rol     tmp1
        rol     sreg
        rol     sreg+1
        dex
        bne     @L1
        ldx     tmp1
        rts

