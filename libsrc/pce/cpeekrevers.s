;
; 2020-07-14, Groepaz
;
; unsigned char cpeekrevers (void);
;
; get inverse flag from current position, do NOT advance cursor

        .export         _cpeekrevers

        .include        "pce.inc"
        .include        "extzp.inc"

_cpeekrevers:
        st0     #VDC_MARR       ; Memory-Address Read
        ldy     SCREEN_PTR
        ldx     SCREEN_PTR+1
        sty     VDC_DATA_LO
        stx     VDC_DATA_HI

        st0     #VDC_VRR        ; VRAM Read Register
        lda     VDC_DATA_LO     ; character (bit 7 is revers bit)
        rol     a
        rol     a
        and     #1
        ldx     #0
        rts
