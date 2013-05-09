;
; Ullrich von Bassewitz, 06.08.1998
;
; CC65 runtime: Scale the 32 bit primary register by 4
;

        .export         asleax2, shleax2
        .importzp       sreg, tmp1

asleax2:
shleax2:
        stx     tmp1
        asl     a
        rol     tmp1
        rol     sreg
        rol     sreg+1
        asl     a
        rol     tmp1
        rol     sreg
        rol     sreg+1
        ldx     tmp1
        rts

