;
; 2020-07-14, Groepaz
;
; char cpeekc (void);
;
; get character from current position, do NOT advance cursor

        .export         _cpeekc

        .include        "pce.inc"
        .include        "extzp.inc"

_cpeekc:
        st0     #VDC_MARR       ; Memory-Address Read
        ldy     SCREEN_PTR
        ldx     SCREEN_PTR+1
        sty     VDC_DATA_LO
        stx     VDC_DATA_HI

        st0     #VDC_VRR        ; VRAM Read Register
        lda     VDC_DATA_LO     ; character
        and     #<~$80          ; remove reverse bit
        ldx     #0
        rts
