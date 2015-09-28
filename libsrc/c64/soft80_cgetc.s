;
; char cgetc (void);
;

        .export         soft80_cgetc
        .import         cursor
        .import         soft80_putcolor

        .include        "c64.inc"
        .include        "soft80.inc"

soft80_cgetc:
        lda     KEY_COUNT       ; Get number of characters
        bne     L3              ; Jump if there are already chars waiting

; Switch on the cursor if needed

        lda     cursor
        jsr     setcursor       ; set cursor on or off accordingly

L1:     lda     KEY_COUNT       ; wait for key
        beq     L1

        ldx     #0
        lda     CURS_FLAG
        bne     L2
        inx
L2:     txa
        jsr     setcursor

L3:     jsr     KBDREAD         ; Read char and return in A
        ldx     #0
        rts

; Switch the cursor on or off

; A= 0: cursor off
;    1: cursor on

        .proc   setcursor

        ; On or off?
        cmp     CURS_STATE
        bne     @set
        rts
@set:
        sta     CURS_STATE

        sei
        lda     $01
        pha
        lda     #$34
        sta     $01

        jsr     soft80_putcolor

        ldy     #$00

        lda     CURS_X
        and     #$01
        bne     @l1

        .repeat 8,line
        lda     (SCREEN_PTR),y
        eor     #$f0
        sta     (SCREEN_PTR),y
        .if (line < 7)
        iny
        .endif
        .endrepeat

@back:
        pla
        sta     $01
        cli
        rts

@l1:
        .repeat 8,line
        lda     (SCREEN_PTR),y
        eor     #$0f
        sta     (SCREEN_PTR),y
        .if line < 7
        iny
        .endif
        .endrepeat

        jmp     @back

        .endproc
