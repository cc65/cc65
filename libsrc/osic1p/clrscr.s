;
; void clrscr (void);
;
        .export         _clrscr
        .import         plot, _memset, pushax
        .import         ScrBase, ScrRamSize
        .include        "extzp.inc"
        .include        "osic1p.inc"

_clrscr:
        lda     ScrBase         ; Fill whole video RAM with blanks by calling
        ldx     ScrBase+1       ; memset appropriately
        jsr     pushax
        ldx     #$00
        lda     #' '
        jsr     pushax
        lda     ScrRamSize
        ldx     ScrRamSize+1
        jsr     _memset

        lda     #$00            ; Cursor in upper left corner
        sta     CURS_X
        sta     CURS_Y
        jmp     plot            ; Set the cursor position
