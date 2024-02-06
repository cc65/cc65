;
; Ullrich von Bassewitz, 06.08.1998
;
; char cgetc (void);
;

        .export         _cgetc
        .import         cursor

        .include        "pet.inc"

_cgetc: lda     KEY_COUNT       ; Get number of characters
        bne     L3              ; Jump if there are already chars waiting

; Switch on the cursor if needed

        lda     CURS_FLAG
        pha
        lda     cursor
        jsr     setcursor
L1:     lda     KEY_COUNT
        beq     L1
        ldx     #0
        pla
        bne     L2
        inx
L2:     txa
        jsr     setcursor

; Fetch the character from the keyboard buffer

L3:     sei
        ldy     KEY_BUF
        ldx     #$00
L4:     lda     KEY_BUF+1,x
        sta     KEY_BUF,x
        inx
        cpx     KEY_COUNT
        bne     L4
        dec     KEY_COUNT
        cli
        ldx     #$00            ; Clear high byte
        tya
        rts

; Switch the cursor on or off

setcursor:
        tax                     ; On or off?
        bne     seton           ; Go set it on
        lda     CURS_FLAG       ; Is the cursor currently off?
        bne     crs9            ; Jump if yes
        lda     #1
        sta     CURS_FLAG       ; Mark it as off
        lda     CURS_STATE      ; Cursor currently displayed?
        beq     crs8            ; Jump if no
        ldy     CURS_X          ; Get the character column
        lda     (SCREEN_PTR),y  ; Get character
        eor     #$80
        sta     (SCREEN_PTR),y  ; Store character back
crs8:   lda     #0
        sta     CURS_STATE      ; Cursor not displayed
crs9:   rts

seton:  lda     #0
        sta     CURS_FLAG
        rts

