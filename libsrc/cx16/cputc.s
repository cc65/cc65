;
; 2019-09-23, Greg King
;
; void __fastcall__ cputcxy (unsigned char x, unsigned char y, char c);
; void __fastcall__ cputc (char c);
;

        .export         _cputcxy, _cputc, cputdirect, putchar
        .export         newline, plot

        .import         gotoxy, PLOT

        .include        "cx16.inc"
        .macpack        generic


; First, move to a new position.

_cputcxy:
        pha                     ; Save C
        jsr     gotoxy          ; Set cursor, drop x and y
        pla                     ; Restore C

; Print a character.

_cputc: cmp     #$0D            ; LF?
        beq     newline
        cmp     #$0A            ; CR?
        beq     plotx0

; Printable char of some sort

        cmp     #' '
        blt     cputdirect      ; Other control char
        tay
        bmi     L10
        cmp     #$60
        blt     L2
        and     #<~%00100000
        bra     cputdirect

; Handle character if high bit set

L10:    and     #<~%10000000    ; Remove high bit
        ora     #%01000000
        bra     cputdirect

L2:     and     #<~%01000000

cputdirect:
        jsr     putchar         ; Write character to screen, return .Y

; Advance cursor position.

        iny
        cpy     LLEN            ; Reached end of line?
        bne     L3
        jsr     newline         ; Next line
        ldy     #$00            ; + CR
L3:     sty     CURS_X
        rts

; Move down.

newline:
        inc     SCREEN_PTR+1
        inc     CURS_Y
        rts


; Set the cursor's position, calculate RAM pointer.

plotx0: stz     CURS_X
plot:   ldy     CURS_X
        ldx     CURS_Y
        clc
        jmp     PLOT            ; Set the new cursor


; Write one screen-code and color to the video RAM without doing anything else.
; Return the x position in .Y .

putchar:
        ora     RVS             ; Set revers bit
        tax
        stz     VERA::CTRL      ; Use port 0
        lda     CURS_Y
        sta     VERA::ADDR+1    ; Set row number
        lda     #VERA::INC1     ; Address increments by one
        sta     VERA::ADDR+2
        ldy     CURS_X          ; Get character column into .Y
        tya
        asl     a               ; Each character has two bytes
        sta     VERA::ADDR
        stx     VERA::DATA0
        lda     CHARCOLOR
        sta     VERA::DATA0
        rts
