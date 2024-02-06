;
; Ullrich von Bassewitz, 06.08.1998
;
; CC65 runtime: Scale the primary register by 8
;

        .export         aslax3, shlax3
        .importzp       tmp1

aslax3:
shlax3: stx     tmp1
        asl     a
        rol     tmp1
        asl     a
        rol     tmp1
        asl     a
        rol     tmp1
        ldx     tmp1
        rts

