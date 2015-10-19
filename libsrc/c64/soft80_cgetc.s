;
; Groepaz/Hitmen, 11.10.2015
;
; high level implementation for the soft80 implementation
;
; char cgetc (void);
;

        .export         soft80_cgetc
        .import         soft80_internal_cellcolor, soft80_internal_cursorxlsb
        .import         cursor
        .importzp       tmp1

        .include        "c64.inc"
        .include        "soft80.inc"

soft80_cgetc:
        lda     KEY_COUNT       ; Get number of characters
        bne     @L3             ; Jump if there are already chars waiting

        sec
        jsr     invertcursor    ; set cursor on or off accordingly

@L1:    lda     KEY_COUNT       ; wait for key
        beq     @L1

        clc
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
        jsr     setcolor

        ldx     soft80_internal_cursorxlsb
@lp1:
        lda     (SCREEN_PTR),y
        eor     nibble,x
        sta     (SCREEN_PTR),y
        iny
        cpy     #8
        bne     @lp1

        pla
        sta     $01             ; enable I/O
        cli
        rts

        ; do not use soft80_putcolor here to make sure the cursor is always
        ; shown using the current textcolor without disturbing the "color voodoo"
        ; in soft80_cputc
setcolor:
        ;ldy     #0              ; is 0
        bcs     @set
        ; restore old value
        lda     tmp1
        sta     (CRAM_PTR),y    ; vram
        rts
@set:
        ; save old value
        lda     (CRAM_PTR),y    ; vram
        sta     tmp1
        lda     soft80_internal_cellcolor
        sta     (CRAM_PTR),y    ; vram
        rts

        .rodata
nibble: .byte $f0, $0f

;-------------------------------------------------------------------------------
; force the init constructor to be imported

        .import soft80_init
conio_init      = soft80_init
