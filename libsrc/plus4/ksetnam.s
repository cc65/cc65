;
; Ullrich von Bassewitz, 22.11.2002
;
; SETNAM replacement function
;

        .export         SETNAM

        .include        "plus4.inc"

; This function is special in that the name must reside in low memory,
; otherwise it is not accessible by the ROM code.

.segment        "LOWCODE"               ; Must go into low memory

.proc   SETNAM

; Store the length of the name into the zero page

        sta     FNAM_LEN

; Check if we have to copy the name to low memory

        cmp     #$00                    ; Length zero?
        beq     @L3                     ; Yes: Copying not needed
        cpy     #$00                    ; Is the name in low memory?
        bpl     @L3                     ; Yes: Copying not needed

; Store the length and the pointer to the name

        stx     TMPPTR
        sty     TMPPTR+1                ; Store pointer to name in TMPPTR

; Copy the given name into FNBUF.

        ldy     #$00
@L2:    lda     (TMPPTR),y
        sta     FNBUF,y
        iny
        cpy     FNAM_LEN
        bne     @L2

; Load the new parameters for the low memory buffer

        ldx     #<FNBUF
        ldy     #>FNBUF

; Instead of banking in the ROM, store the values directly into the zeropage

@L3:    stx     FNAM
        sty     FNAM+1

; Return to caller

        rts

.endproc


