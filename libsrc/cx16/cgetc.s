;
; 2019-12-22, Greg King
;
; char cgetc (void);
; /* Return a character from the keyboard. */
;

        .export         _cgetc

        .import         _kbhit, cursor, GETIN

        .include        "cx16.inc"
        .macpack        generic


_cgetc: jsr     _kbhit
        bnz     L3              ; Jump if there are already chars waiting

; Switch the cursor on if wanted.

        lda     CURS_FLAG       ; Save cursor's current enable flag
        tay
        lda     cursor
        jsr     setcursor
L1:     jsr     _kbhit
        bze     L1              ; Wait for key
        tya
        eor     #%00000001      ; (Cursor flag uses negative logic)
        jsr     setcursor       ; Restore previous cursor condition

; An internal Kernal function can't be used because it might be moved in future
; revisions.  Use an official function; but, make sure that it reads
; the keyboard.

L3:     ldy     IN_DEV          ; Save current input device
        stz     IN_DEV          ; Keyboard
        phy
        jsr     GETIN           ; Read char, and return in .A
        ply
        sty     IN_DEV          ; Restore input device
        ldx     #>$0000
        rts

; Switch the cursor on or off.

setcursor:
        tax                     ; On or off?
        bnz     seton           ; Go set it on
        lda     CURS_FLAG       ; Is the cursor currently off?
        bnz     crs9            ; Jump if yes
        inc     CURS_FLAG       ; Mark it as off
        ldx     CURS_STATE      ; Cursor currently displayed?
        bze     crs9            ; Jump if not

; Restore the current character in video RAM.
; Restore that character's colors.

        stz     VERA::CTRL      ; Use port 0
        lda     CURS_Y
        sta     VERA::ADDR+1    ; Set row number
        lda     #VERA::INC1     ; Increment address by one
        sta     VERA::ADDR+2
        lda     CURS_X          ; Get character column
        asl     a
        sta     VERA::ADDR
        ldx     CURS_CHAR
        stx     VERA::DATA0
        ldx     CURS_COLOR
        stx     VERA::DATA0
        stz     CURS_STATE      ; Cursor not displayed
crs9:   rts

seton:  stz     CURS_FLAG
        rts
