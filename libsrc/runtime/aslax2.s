;
; Ullrich von Bassewitz, 06.08.1998
;
; CC65 runtime: Scale the primary register by 4
;

        .export         aslax2, shlax2
        .importzp       tmp1

aslax2:
shlax2: stx     tmp1
        asl     a
        rol     tmp1
        asl     a
        rol     tmp1
        ldx     tmp1
        rts

