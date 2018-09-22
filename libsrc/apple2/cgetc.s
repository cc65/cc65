;
; Kevin Ruland
;
; char cgetc (void);
;
; If open_apple key is pressed then the high-bit of the key is set.
;

        .export         _cgetc
        .import         cursor, putchardirect

        .include        "apple2.inc"

_cgetc:
        ; Cursor on ?
        lda     cursor
        beq     :+

        ; Show caret.
        .ifdef  __APPLE2ENH__
        lda     #$7F | $80      ; Checkerboard, screen code
        .else
        lda     #' ' | $40      ; Blank, flashing
        .endif
        jsr     putchardirect   ; Returns old character in X

        ; Wait for keyboard strobe.
:       inc     RNDL            ; Increment random counter low
        bne     :+
        inc     RNDH            ; Increment random counter high
:       lda     KBD
        bpl     :--             ; If < 128, no key pressed

        ; Cursor on ?
        ldy     cursor
        beq     :+

        ; Restore old character.
        pha
        txa
        jsr     putchardirect
        pla

        ; At this time, the high bit of the key pressed is set.
:       bit     KBDSTRB         ; Clear keyboard strobe
        .ifdef __APPLE2ENH__
        bit     BUTN0           ; Check if OpenApple is down
        bmi     done
        .endif
        and     #$7F            ; If not down, then clear high bit
done:   ldx     #>$0000
        rts
