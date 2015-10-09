;
; char cgetc (void);
;

        .export         soft80_cgetc
        .import         cursor

        .include        "c64.inc"
        .include        "soft80.inc"

.if SOFT80COLORVOODOO = 1
        .import         soft80_putcolor
.endif

soft80_cgetc:
        lda     KEY_COUNT       ; Get number of characters
        bne     L3              ; Jump if there are already chars waiting

        jsr     invertcursor    ; set cursor on or off accordingly

L1:     lda     KEY_COUNT       ; wait for key
        beq     L1

        jsr     invertcursor    ; set cursor on or off accordingly

L3:     jsr     KBDREAD         ; Read char and return in A
        ldx     #0
        rts

; Switch the cursor on or off (invert)

invertcursor:
        lda     cursor
        bne     @invert
        rts
@invert:

        sei
        lda     $01
        pha
        lda     #$34            ; enable RAM under I/O
        sta     $01

        ldy     #$00
.if SOFT80COLORVOODOO = 1
        jsr     soft80_putcolor
.else
        lda     CHARCOLOR
        sta     (CRAM_PTR),y    ; vram
.endif

        lda     CURS_X
        and     #$01
        tax
@lp1:
        lda     (SCREEN_PTR),y
        eor     nibble,x
        sta     (SCREEN_PTR),y
        iny
        cpy     #8
        bne     @lp1

        pla
        sta     $01
        cli
        rts

        .rodata
nibble: .byte $f0, $0f

;-------------------------------------------------------------------------------
; force the init constructor to be imported

        .import soft80_init
conio_init      = soft80_init
