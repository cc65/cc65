;
; Ullrich von Bassewitz, 2003-04-13
;

	.export	  	_clrscr
        .importzp       ptr2

	.include  	"atmos.inc"

; ------------------------------------------------------------------------
; void __fastcall__ clrscr (void);

.proc	_clrscr

; Set the cursor to top left cursor position

        ldy     #$00
        sty     CURS_X
        sty     CURS_Y

; Set ptr2 to the screen position (left upper border)

        lda     #<SCREEN
        sta     ptr2
        lda     #>SCREEN
        sta     ptr2+1

; Clear full pages. Y is still zero

        ldx     #>(28*40)
        lda     #' '
@L1:    sta     (ptr2),y
        iny                     ; Bump low byte of address
        bne     @L1
        inc     ptr2+1          ; Bump high byte of address
        dex
        bne     @L1

; Clear the remaining page

@L2:    sta     (ptr2),y
        iny
        cpy     #<(28*40)
        bne     @L2
        rts

.endproc
