;
; Christian Groessler, June 2000
;
; mul40
; multiplies A by 40 and returns result in AX
; uses tmp4

        .importzp       tmp4
        .export         mul40,loc_tmp

.proc   mul40

        ldx     #0
        stx     tmp4
        sta     loc_tmp
        asl     a
        rol     tmp4
        asl     a
        rol     tmp4            ; val * 4
        adc     loc_tmp
        bcc     L1
        inc     tmp4            ; val * 5
L1:     asl     a
        rol     tmp4            ; val * 10
        asl     a
        rol     tmp4
        asl     a
        rol     tmp4            ; val * 40
        ldx     tmp4
        rts

.endproc

        .bss
loc_tmp:.res    1
