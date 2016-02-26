;
; Macro definitions for screen layout modules
;

        .include        "extzp.inc"
        
.linecont +

;
; Internal function for screensize()
;
.macro  osi_screensize ScrWidth, ScrHeight
                                ; Macro implementation of internal screensize
                                ; function for given width and height in
                                ; characters
                                
        .export         screensize

.proc   screensize
        ldx     #ScrWidth
        ldy     #ScrHeight
        rts
.endproc
.endmacro

;
; void clrscr (void);
;
.macro  osi_clrscr ScrBase, ScrRamSize

        .export         _clrscr

.proc   _clrscr
        lda     #<ScrBase       ; Fill whole video RAM with blanks by calling
        ldx     #>ScrBase       ; memset appropriately
        jsr     pushax
        
        lda     #' '
        ldx     #$00
        jsr     pushax
        
        lda     #<ScrRamSize
        ldx     #>ScrRamSize
        jsr     _memset

        lda     #$00            ; Cursor in upper left corner
        sta     CURS_X
        sta     CURS_Y
        
        jmp     plot            ; Set the cursor position
.endproc

.endmacro

;
; cputc/cputcxy for Challenger 1P
; Based on PET/CBM implementation
;

.macro  osi_cputfuncs ScrBase, ScrFirstChar, ScrWidth, ScrHeight, \
                         ScrollDist, ScrLo, ScrHi

                                ; Number of characters to move for scrolling
                                ; by one line
ScrollLength    = (ScrHeight - 1) * ScrollDist

;
; void cputcxy (unsigned char x, unsigned char y, char c);
; void cputc (char c);
;
        .export         _cputcxy, _cputc, cputdirect, putchar
        .export         newline, plot

_cputcxy:
        pha                     ; Save C
        jsr     popa            ; Get Y
        jsr     _gotoxy         ; Set cursor, drop x
        pla                     ; Restore C

; Plot a character - also used as internal function

_cputc: cmp     #$0A            ; CR?
        bne     L1
        lda     #0
        sta     CURS_X
        beq     plot            ; Recalculate pointers

L1:     cmp     #$0D            ; LF?
        beq     newline         ; Recalculate pointers

cputdirect:
        jsr     putchar         ; Write the character to the screen

; Advance cursor position, register Y contains horizontal position after
; putchar

        cpy     #(ScrWidth - 1) ; Check whether line is full
        bne     L3
        jsr     newline         ; New line
        ldy     #$FF            ; + cr
L3:     iny
        sty     CURS_X
        rts

newline:
        inc     CURS_Y
        lda     CURS_Y
        cmp     #ScrHeight      ; Screen height
        bne     plot
        dec     CURS_Y          ; Bottom of screen reached, scroll

                                ; Scroll destination address
        lda     #<(ScrBase + ScrFirstChar)
        ldx     #>(ScrBase + ScrFirstChar)
        jsr     pushax
        
                                ; Scroll source address
        lda     #<(ScrBase + ScrFirstChar + ScrollDist)
        ldx     #>(ScrBase + ScrFirstChar + ScrollDist)
        jsr     pushax
        
                                ; Number of characters to move
        lda     #<ScrollLength
        ldx     #>ScrollLength
        jsr     _memmove

                                ; Address of first character in last line
                                ; of screen
        lda     #<(ScrBase + ScrFirstChar + ScrollLength)
        sta     ptr1
        lda     #>(ScrBase + ScrFirstChar + ScrollLength)
        sta     ptr1+1
        
        ldy     #ScrWidth       ; Fill last line with blanks
        lda     #' '
clrln:  sta     (ptr1),y
        dey
        bpl     clrln

plot:   ldy     CURS_Y
        lda     ScrLo,y
        sta     SCREEN_PTR
        lda     ScrHi,y
        sta     SCREEN_PTR+1
        rts

; Write one character to the screen without doing anything else, return X
; position in register Y

putchar:
        ldy     CURS_X
        sta     (SCREEN_PTR),y  ; Set char
        rts
        
.endmacro

.macro osi_screen_funcs ScrBase, ScrRamSize, ScrFirstChar, \
                        ScrWidth, ScrHeight, ScrollDist

        .import         popa, _gotoxy
        .import         _memmove, _memset, pushax
        .importzp       ptr1

.rodata

; Screen address tables - offset to real screen
ScrTabLo:
        .repeat ScrHeight, I
        .byte <(ScrBase + ScrFirstChar + I * ScrollDist)
        .endrep
        
ScrTabHi:
        .repeat ScrHeight, I
        .byte >(ScrBase + ScrFirstChar + I * ScrollDist)
        .endrep

.code

osi_cputfuncs   ScrBase, ScrFirstChar, ScrWidth, ScrHeight, \
                         ScrollDist, ScrTabLo, ScrTabHi
osi_screensize  ScrWidth, ScrHeight
osi_clrscr      ScrBase, ScrRamSize

.endmacro