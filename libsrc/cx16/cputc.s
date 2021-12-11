;
; 2020-10-13, Greg King
;
; void __fastcall__ cputcxy (unsigned char x, unsigned char y, char c);
; void __fastcall__ cputc (char c);
;

        .export         _cputcxy, _cputc, cputdirect, putchar
        .export         newline, plot

        .import         gotoxy, PLOT

        .include        "cx16.inc"


; Move to a cursor position, then print a character.

_cputcxy:
        pha                     ; Save C
        jsr     gotoxy          ; Set cursor, drop x and y
        pla

; Print a character -- also used as an internal function.

_cputc: cmp     #$0D            ; X16 '\n'?
        beq     newline
        cmp     #$0A            ; X16 '\r'?
        beq     cr

; Printable char. of some sort.
; Convert it from PetSCII into a screen-code.

convert:
        tay
        lsr     a               ; Divide by 256/8
        lsr     a
        lsr     a
        lsr     a
        lsr     a
        tax                     ; .X = %00000xxx
        tya
        eor     pet_to_screen,x

cputdirect:
        jsr     putchar         ; Write character to screen, return .Y

; Advance the cursor position.

        iny
        cpy     LLEN            ; Reached end of line?
        bne     L3
        jsr     newline         ; Wrap around

cr:     ldy     #$00
L3:     sty     CURS_X
        rts

; Move down.

newline:
        inc     SCREEN_PTR+1
        inc     CURS_Y
        rts


; Set the cursor's position, calculate RAM pointer.

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


        .rodata
pet_to_screen:
        .byte %10000000,%00000000,%01000000,%00100000  ; PetSCII -> screen-code
        .byte %01000000,%11000000,%10000000,%10000000
