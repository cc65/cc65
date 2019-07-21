; 2018-04-13, Jede (jede@oric.org)
;

; void cputc (char c);
;

        .export         _cputc, CHARCOLOR, OLD_CHARCOLOR, BGCOLOR, OLD_BGCOLOR

        .include        "telestrat.inc"

.proc _cputc
    ldx     CHARCOLOR
    cpx     OLD_CHARCOLOR
    beq     do_not_change_color_foreground
    
    stx     OLD_CHARCOLOR         ; Store CHARCOLOR into OLD_CHARCOLOR

    dec     SCRX
    dec     SCRX

    pha
    txa                           ; Swap X to A because, X contains CHARCOLOR
    BRK_TELEMON  XFWR             ; Change color on the screen (foreground)
    inc     SCRX
    pla

do_not_change_color_foreground:
    ldx     BGCOLOR
    cpx     OLD_BGCOLOR
    beq     do_not_change_color

    stx     OLD_BGCOLOR

    dec     SCRX                 ; Dec SCRX in order to place attribute before the right position

    pha
    txa                          ; Swap X to A because, X contains BGCOLOR
    ORA     #%00010000           ; Add 16 because background color is an attribute between 16 and 23. 17 is red background for example
    BRK_TELEMON  XFWR            ; Change color on the screen (background)
    pla

do_not_change_color:
    BRK_TELEMON  XFWR            ; Macro send char to screen (channel 0)
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
