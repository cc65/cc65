;
; 2020-04-30, Greg King
;
; unsigned char cpeekcolor (void);
; /* Return the colors from the current cursor position. */
;

        .export         _cpeekcolor

        .include        "cx16.inc"


_cpeekcolor:
        php
        lda     CURS_FLAG       ; is the cursor currently off?
        bne     @L1
        sei                     ; don't let cursor blinking interfere
        ldx     CURS_STATE      ; is cursor currently displayed?
        beq     @L1             ; jump if not
        lda     CURS_COLOR      ; get color under cursor
        bra     @L2

@L1:    stz     VERA::CTRL      ; use port 0
        lda     CURS_Y
        sta     VERA::ADDR+1    ; set row number
        stz     VERA::ADDR+2
        lda     CURS_X          ; get character column
        sec                     ; color attribute is second byte
        rol     a
        sta     VERA::ADDR
        lda     VERA::DATA0     ; get color of character
@L2:    plp
        ldx     #>$0000
        rts
