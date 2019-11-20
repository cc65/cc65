;
; 2016-02-28, Groepaz
; 2019-09-25, Greg King
;
; unsigned char cpeekrevers (void);
; /* Return the reverse attribute from the current cursor position.
; ** If the character is reversed, then return 1; return 0 otherwise.
; */
;

        .export         _cpeekrevers

        .include        "cx16.inc"


_cpeekrevers:
        php
        sei                     ; don't let cursor blinking interfere
        stz     VERA::CTRL      ; use port 0
        lda     CURS_Y
        sta     VERA::ADDR+1    ; set row number
        stz     VERA::ADDR+2
        lda     CURS_X          ; get character column
        asl     a               ; each character has two bytes
        sta     VERA::ADDR
        lda     VERA::DATA0     ; get screen code
        plp
        and     #%10000000      ; get reverse bit
        asl     a
        tax                     ; ldx #>$0000
        rol     a               ; return boolean value
        rts
