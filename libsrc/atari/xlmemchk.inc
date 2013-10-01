;
; Christian Groessler, Jun-2013
;
; This routine is used in preparation to move the screen memory
; in front of the program.
;
; It calculates the value to put into RAMTOP for a subsequent
; "GRAPHICS 0" call, and the lowest address which will be used
; by the screen memory afterwards.
; 
; inputs:
;       __STARTADDRESS__        -       load address of the program
; outputs:
;       lodadr                  -       (high byte only) value to
;                                       write into RAMTOP
;       lowadr                  -       lowest address occupied by
;                                       screen data
;


; When setting a display mode, the ROM takes the RAMTOP value
; and subtracts the size of the screen memory from it. This will
; become the new screen memory address.
; From this address it subtracts the size of the display list.
; This will become the new display list address.
; Screen memory cannot cross 4K boundaries and a display list
; cannot cross a 1K boundary.
;
; Work out a sane value for RAMTOP to prevent boundary crossing.
; RAMTOP is only one byte, it counts in memory pages.
;
; The ROM doesn't do this boundary checking, since it doesn't
; expect RAMTOP to have (rather) arbitrary values. For a
; "GRAPHICS 0" call and RAMTOP representing the possible physically
; available memory, boundary crossing cannot happen.


SCRBUFSZ =      (40 * 24)               ; size of mode 0 screen buffer
DLSZ    =       32                      ; size of mode 0 display list


scrmemtst:

; subtract screen memory size from our load address

        lda     lodadr
        sec
        sbc     #<SCRBUFSZ
        sta     tstadr
        lda     lodadr+1
        sbc     #>SCRBUFSZ
        sta     tstadr+1

; check if a 4K boundary is crossed

        lda     lodadr+1
        and     #$f0
        sta     tmp
        lda     tstadr+1
        and     #$f0
        cmp     tmp
        beq     scrmemok

; if lodadr is at an exact 4K boundary, it's still ok

        lda     lodadr+1
        and     #$0f
        beq     scrmemok

; 4K boundary will be crossed, use this 4K boundary address as lodadr

al4k:   lda     lodadr+1
        and     #$f0
        sta     lodadr+1
        bne     scrmemtst
; not reached

.ifdef DEBUG
.byte "XLMEMCHK:>"
.endif
lodadr: .word   __STARTADDRESS__ & $FF00                ; our program's load address, rounded down to page boundary
tstadr: .res    2
lowadr: .res    2
tmp:    .res    1


; subtract display list size from calculated screen address

scrmemok:
        lda     tstadr
        sec
        sbc     #<DLSZ
        sta     lowadr
        lda     tstadr+1
        sbc     #>DLSZ
        sta     lowadr+1

.if 0   ; this cannot happen
; check if a 1K boundary is crossed

        lda     tstadr+1
        and     #$fc
        sta     tmp
        lda     lowadr+1
        and     #$fc
        cmp     tmp
        bne     al4k            ; 1K boundary will be crossed, decrease lodadr
.endif

; address of display list is ok
; decrease lowadr by two

        lda     lowadr
        sec
        sbc     #2
        sta     lowadr
        bcs     dec_cont
        dec     lowadr+1
dec_cont:
