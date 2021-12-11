;
; 1998-08-06, Ullrich von Bassewitz
; 2020-10-09, Greg King
;
; void cputcxy (unsigned char x, unsigned char y, char c);
; void cputc (char c);
;

        .export         _cputcxy, _cputc, cputdirect, putchar
        .export         newline, plot
        .import         gotoxy
        .import         PLOT

.scope  KERNAL
        .include        "cbm_kernal.inc"
.endscope

        .include        "vic20.inc"

; VIC-20 KERNAL routines (such as PLOT) do not always leave the color RAM
; pointer CRAM_PTR pointing at the color RAM location matching the screen
; RAM pointer SCREEN_PTR. Instead they update it when they need it to be
; correct by calling UPDCRAMPTR.
;
; We make things more efficient by having conio always update CRAM_PTR when
; we move the screen pointer to avoid extra calls to ensure it's updated
; before doing screen output. (Among other things, We replace the ROM
; version of PLOT with our own in libsrc/vic20/kplot.s to ensure this
; precondition.)
;
; However, this means that CRAM_PTR may be (and is, after a cold boot)
; incorrect for us at program startup, causing cputc() not to work. We fix
; this with a constructor that ensures CRAM_PTR matches SCREEN_PTR.
;
        UPDCRAMPTR := KERNAL::UPDCRAMPTR    ; .constructor doesn't understand namespaces
        .constructor    UPDCRAMPTR

_cputcxy:
        pha                     ; Save C
        jsr     gotoxy          ; Set cursor, drop x and y
        pla                     ; Restore C

; Plot a character -- also used as an internal function

_cputc: cmp     #$0D            ; Is it CBM '\n'?
        beq     newline         ; Recalculate pointers
        cmp     #$0A            ; Is it CBM '\r'?
        beq     cr

; Printable char. of some sort
; Convert it from PetSCII into a screen-code

        cmp     #$FF            ; BASIC token?
        bne     convert
        lda     #$DE            ; Pi symbol
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
        jsr     putchar         ; Write the character to the screen

; Advance the cursor position

advance:
        iny
        cpy     #XSIZE
        bne     L3
        jsr     newline         ; Wrap around

cr:     ldy     #$00            ; Do carriage-return
L3:     sty     CURS_X
        rts


; Move down by one full screen-line.  Note: this routine doesn't scroll.
;
; (Both screen RAM and color RAM are aligned to page boundaries.
; Therefore, the lower bytes of their addresses have the same values.
; Shorten the code by taking advantage of that fact.)

newline:
        clc
        lda     #XSIZE
        adc     SCREEN_PTR
        sta     SCREEN_PTR
        sta     CRAM_PTR
        bcc     L5
        inc     SCREEN_PTR+1
        inc     CRAM_PTR+1
L5:     inc     CURS_Y
        rts


; Set cursor position, calculate RAM pointers

plot:   ldy     CURS_X
        ldx     CURS_Y
        clc
        jmp     PLOT            ; Set the new cursor


; Write one character to the screen without doing anything else,
; return the X position in .Y

putchar:
        ora     RVS             ; Set revers bit
        ldy     CURS_X
        sta     (SCREEN_PTR),y  ; Set char.
        lda     CHARCOLOR
        sta     (CRAM_PTR),y    ; Set color
        rts


        .rodata
pet_to_screen:
        .byte   %10000000,%00000000,%01000000,%00100000 ; PetSCII -> screen-code
        .byte   %01000000,%11000000,%10000000,%10000000
