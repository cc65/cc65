;
; 2019-09-25, Greg King
;
; unsigned char cpeekcolor (void);
; /* Return the colors from the current cursor position. */
;

        .export         _cpeekcolor

        .include        "cx16.inc"


_cpeekcolor:
        php
        sei                     ; don't let cursor blinking interfere
        stz     VERA::CTRL      ; use port 0
        lda     CURS_Y
        sta     VERA::ADDR+1    ; set row number
        stz     VERA::ADDR+2
        lda     CURS_X          ; get character column
        sec                     ; color attribute is second byte
        rol     a
        sta     VERA::ADDR
        lda     VERA::DATA0     ; get color
        plp
        ldx     #>$0000
        rts
