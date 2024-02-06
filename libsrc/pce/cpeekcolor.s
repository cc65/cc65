;
; 2020-07-14, Groepaz
;
; unsigned char cpeekcolor (void);
;
; get color from current position, do NOT advance cursor

        .export         _cpeekcolor

        .include        "pce.inc"
        .include        "extzp.inc"

_cpeekcolor:
        st0     #VDC_MARR       ; Memory-Address Read
        ldy     SCREEN_PTR
        ldx     SCREEN_PTR+1
        sty     VDC_DATA_LO
        stx     VDC_DATA_HI

        st0     #VDC_VRR        ; VRAM Read Register
        lda     VDC_DATA_HI
        and     #<~$02
        lsr     a
        lsr     a
        lsr     a
        lsr     a
        ldx     #0
        rts
