;
; Groepaz/Hitmen, 19.10.2015
;
; high level implementation for the monochrome soft80 implementation
;
; char cgetc (void);
;

        .export         soft80mono_cgetc
        .import         soft80mono_internal_cellcolor, soft80mono_internal_cursorxlsb
        .import         soft80mono_internal_nibble
        .import         cursor
        .importzp       tmp1

        .include        "c64.inc"
        .include        "soft80.inc"

soft80mono_cgetc:
        lda     KEY_COUNT       ; Get number of characters
        bne     @L3             ; Jump if there are already chars waiting

        jsr     invertcursor    ; set cursor on or off accordingly

@L1:    lda     KEY_COUNT       ; wait for key
        beq     @L1

        jsr     invertcursor    ; set cursor on or off accordingly

@L3:    jsr     KBDREAD         ; Read char and return in A
        ldx     #0
        rts

; Switch the cursor on or off (invert)

invertcursor:
        lda     cursor
        bne     @invert
        rts
@invert:

        sei
        lda     $01             ; enable RAM under I/O
        pha
        lda     #$34
        sta     $01

        ldy     #$00
        ldx     soft80mono_internal_cursorxlsb
@lp1:
        lda     (SCREEN_PTR),y
        eor     soft80mono_internal_nibble,x
        sta     (SCREEN_PTR),y
        iny
        cpy     #8
        bne     @lp1

        pla
        sta     $01             ; enable I/O
        cli
        rts

;-------------------------------------------------------------------------------
; force the init constructor to be imported

        .import soft80mono_init
conio_init      = soft80mono_init
