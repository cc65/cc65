; 2018-04-13, Jede (jede@oric.org)
;

; void cputc (char c);
;

        .export         _cputc, _cputcxy, cputdirect, display_conio
        .export         CHARCOLOR, OLD_CHARCOLOR, BGCOLOR, OLD_BGCOLOR
        
        .import         update_adscr
        .import         popax

        .include        "telestrat.inc"


_cputcxy:
    pha                           ; Save C
    jsr     popax                 ; Get X and Y
    sta     SCRY                  ; Store Y
    stx     SCRX                  ; Store X
    jsr     update_adscr
    pla        

_cputc:
    cmp     #$0D
    bne     @not_CR
    ldy     #$00
    sty     SCRX 
    rts
@not_CR:
    cmp     #$0A
    bne     not_LF

    inc     SCRY
    jmp     update_adscr

cputdirect:
not_LF:
    ldx     CHARCOLOR
    cpx     OLD_CHARCOLOR
    beq     do_not_change_color_foreground

    stx     OLD_CHARCOLOR         ; Store CHARCOLOR into OLD_CHARCOLOR


    pha
    txa                           ; Swap X to A because, X contains CHARCOLOR

    jsr     display_conio

    pla

do_not_change_color_foreground:
    ldx     BGCOLOR
    cpx     OLD_BGCOLOR
    beq     do_not_change_color

    stx     OLD_BGCOLOR

    pha
    txa                          ; Swap X to A because, X contains BGCOLOR
    ora     #%00010000           ; Add 16 because background color is an attribute between 16 and 23. 17 is red background for example

    jsr     display_conio
    pla

do_not_change_color:
    ; it continues to display_conio



.proc display_conio
    ; This routine is used to displays char on screen
    ldy     SCRX
    sta     (ADSCR),y
    iny
    cpy     #SCREEN_XSIZE
    bne     @no_inc
    ldy     #$00
    sty     SCRX

    inc     SCRY
    
    jmp     update_adscr
    
@no_inc:    
    sty     SCRX
    rts
.endproc

.bss
CHARCOLOR:
    .res 1
OLD_CHARCOLOR:
    .res 1
BGCOLOR:
    .res 1
OLD_BGCOLOR:
    .res 1

