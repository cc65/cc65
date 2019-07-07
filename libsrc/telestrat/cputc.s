; 2018-04-13, Jede (jede@oric.org)
;

; void cputc (char c);
;

        .export         _cputc, CHARCOLOR, CHARCOLOR_CHANGE, BGCOLOR, BGCOLOR_CHANGE

        .include        "telestrat.inc"

.proc _cputc
    ldx     CHARCOLOR_CHANGE
    beq     do_not_change_color_foreground
    dec     SCRX
    dec     SCRX
    pha
    lda     CHARCOLOR
    BRK_TELEMON  $4E             ; Change color on the screen (foreground)
    lda     #$00
    sta     CHARCOLOR_CHANGE
    inc     SCRX
    pla

do_not_change_color_foreground:
    ldx     BGCOLOR_CHANGE
    beq     do_not_change_color
    dec     SCRX                 ; Dec SCRX in order to place attribute before the right position
    pha
    lda     BGCOLOR
    ORA     #%00010000           ; Add 16 because background color is an attribute between 16 and 23. 17 is red background for example
    BRK_TELEMON  XFWR            ; Change color on the screen (background)
    lda     #$00
    sta     BGCOLOR_CHANGE

    pla

do_not_change_color:
    BRK_TELEMON  XFWR            ; Macro send char to screen (channel 0)
    rts
.endproc
.bss
CHARCOLOR:
    .res 1
CHARCOLOR_CHANGE:
    .res 1    
BGCOLOR:
    .res 1
BGCOLOR_CHANGE:
    .res 1    
