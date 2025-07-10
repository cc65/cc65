;
; 2017-06-21, Greg King
;
; char cpeekc (void);
;
; Get a character from OSI C1P screen RAM.
;
        .export         _cpeekc

        .include        "extzp.inc"


_cpeekc:
        ldy     CURS_X
        lda     (SCREEN_PTR),y
        ldx     #>$0000
        rts
