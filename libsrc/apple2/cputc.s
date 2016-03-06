;
; Ullrich von Bassewitz, 06.08.1998
;
; void __fastcall__ cputcxy (unsigned char x, unsigned char y, char c);
; void __fastcall__ cputc (char c);
;

        .ifdef  __APPLE2ENH__
        .constructor    initconio
        .endif
        .export         _cputcxy, _cputc
        .export         cputdirect, newline, putchar
        .import         popa, _gotoxy, VTABZ

        .include        "apple2.inc"

        .segment        "ONCE"

        .ifdef  __APPLE2ENH__
initconio:
        sta     SETALTCHAR      ; Switch in alternate charset
        bit     LORES           ; Limit SET80COL-HISCR to text
        rts
        .endif

        .code

; Plot a character - also used as internal function

_cputcxy:
        pha                     ; Save C
        jsr     popa            ; Get Y
        jsr     _gotoxy
        pla                     ; Restore C

_cputc:
        cmp     #$0D            ; Test for \r = carrage return
        beq     left
        cmp     #$0A            ; Test for \n = line feed
        beq     newline
        ora     #$80            ; Turn on high bit
        .ifndef __APPLE2ENH__
        cmp     #$E0            ; Test for lowercase
        bcc     cputdirect
        and     #$DF            ; Convert to uppercase
        .endif

cputdirect:
        jsr     putchar
        inc     CH              ; Bump to next column
        lda     CH
        cmp     WNDWDTH
        bcc     :+
left:   lda     #$00            ; Goto left edge of screen
        sta     CH
:       rts

newline:
        inc     CV              ; Bump to next line
        lda     CV
        cmp     WNDBTM
        bcc     :+
        lda     WNDTOP          ; Goto top of screen
        sta     CV
:       jmp     VTABZ
                
putchar:
        .ifdef  __APPLE2ENH__
        ldy     INVFLG
        cpy     #$FF            ; Normal character display mode?
        beq     put
        cmp     #$E0            ; Lowercase?
        bcc     mask
        and     #$7F            ; Inverse lowercase
        bra     put
        .endif
mask:   and     INVFLG          ; Apply normal, inverse, flash
put:    ldy     CH
        .ifdef  __APPLE2ENH__
        bit     RD80VID         ; In 80 column mode?
        bpl     col40           ; No, in 40 cols
        pha
        tya
        lsr                     ; Div by 2
        tay
        pla
        bcs     col40           ; Odd cols go in 40 col memory
        bit     HISCR           ; Assume SET80COL
        sta     (BASL),Y
        bit     LOWSCR          ; Assume SET80COL
        rts
        .endif
col40:  sta     (BASL),Y
        rts
