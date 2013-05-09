;
; Ullrich von Bassewitz, 06.08.1998
;
; CC65 runtime: Scale the primary register
;

        .export         aslax1, shlax1
        .importzp       tmp1
                            
aslax1:
shlax1: stx     tmp1
        asl     A
        rol     tmp1
        ldx     tmp1
        rts


