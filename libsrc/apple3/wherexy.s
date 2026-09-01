;
; Robert Justice, 2026
;
; unsigned char wherex (void);
; unsigned char wherey (void);
;

        .export         _wherex, _wherey, getpos, cursorx, cursory
        .import         consdev

        .include        "apple3.inc"
        .include        "sos.inc"

_wherex:
        jsr     getpos
        lda     curspos
        ldx     #>$0000
        rts

_wherey:
        jsr     getpos
        lda     curspos+1
        ldx     #>$0000
        rts

; Get cursor position
getpos: lda     consdev
        sta     sosparam + SOS::D_STAT_CTRL::DEV_NUM
        lda     #STAT_CURSOR_POS
        sta     sosparam + SOS::D_STAT_CTRL::CODE
        lda     #<curspos
        ldx     #>curspos
        sta     sosparam + SOS::D_STAT_CTRL::LIST
        stx     sosparam + SOS::D_STAT_CTRL::LIST+1
        lda     #D_STATUS_CALL
        ldx     #D_STATUS_COUNT
        jsr     callsos
        rts

        .data

curspos: 
cursorx:  .byte   00            ; horiz
cursory:  .byte   00            ; vert
