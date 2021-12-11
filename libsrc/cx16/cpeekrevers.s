;
; 2016-02-28, Groepaz
; 2020-04-30, Greg King
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
        lda     CURS_FLAG       ; is the cursor currently off?
        bne     @L1
        sei                     ; don't let cursor blinking interfere
        ldx     CURS_STATE      ; is cursor currently displayed?
        beq     @L1             ; jump if not
        lda     CURS_CHAR       ; get screen code under cursor
        bra     @L2

@L1:    stz     VERA::CTRL      ; use port 0
        lda     CURS_Y
        sta     VERA::ADDR+1    ; set row number
        stz     VERA::ADDR+2
        lda     CURS_X          ; get character column
        asl     a               ; each character has two bytes
        sta     VERA::ADDR
        lda     VERA::DATA0     ; get screen code
@L2:    plp
        and     #%10000000      ; get reverse bit
        asl     a
        tax                     ; ldx #>$0000
        rol     a               ; return boolean value
        rts
