;
; Kevin Ruland
;
; char cgetc (void);
;
; If open_apple key is pressed then the high-bit of the key is set.
;

        .export         _cgetc

        .ifndef __APPLE2ENH__
        .import         iie_or_newer
        .endif
        .import         cursor, putchardirect

        .include        "zeropage.inc"
        .include        "apple2.inc"

_cgetc:
        ; Cursor on ?
        lda     cursor
        beq     :+

        ; Show caret.
        .ifndef __APPLE2ENH__
        lda     #' ' | $40      ; Blank, flashing
        bit     iie_or_newer
        bpl     put_caret
        .endif

        lda     #$7F | $80      ; Checkerboard, screen code
put_caret:
        jsr     putchardirect   ; Saves old character in tmp3

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
        lda     tmp3
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
