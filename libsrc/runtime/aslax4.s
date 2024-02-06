;
; Ullrich von Bassewitz, 25.07.2001
;
; CC65 runtime: Scale the primary register by 16
;

        .export         aslax4, shlax4
        .importzp       tmp1

aslax4:
shlax4: stx     tmp1
        asl     a
        rol     tmp1
        asl     a
        rol     tmp1
        asl     a
        rol     tmp1
        asl     a
        rol     tmp1
        ldx     tmp1
        rts

