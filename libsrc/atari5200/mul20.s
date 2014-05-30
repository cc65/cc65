;
; Christian Groessler, April 2014
;
; mul20
; multiplies A by 20 and returns result in AX
; uses tmp4

        .importzp       tmp4
        .export         mul20,loc_tmp

.proc   mul20

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
        rol     tmp4            ; val * 20
        ldx     tmp4
        rts

.endproc

        .bss
loc_tmp:.res    1
