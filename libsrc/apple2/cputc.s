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
        .export         cputdirect, newline, putchar, putchardirect
        .import         gotoxy, VTABZ
        .ifndef __APPLE2ENH__
        .import         uppercasemask
        .endif

        .include        "apple2.inc"

        .macpack        cpu

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
        jsr     gotoxy          ; Call this one, will pop params
        pla                     ; Restore C and run into _cputc

_cputc:
        cmp     #$0D            ; Test for \r = carrage return
        beq     left
        cmp     #$0A            ; Test for \n = line feed
        beq     newline
        eor     #$80            ; Invert high bit
        .ifndef __APPLE2ENH__
        cmp     #$E0            ; Test for lowercase
        bcc     cputdirect
        and     uppercasemask
        .endif

cputdirect:
        jsr     putchar
        inc     CH              ; Bump to next column
        lda     CH
        cmp     WNDWDTH
        bcc     :+
        jsr     newline
left:
        .if (.cpu .bitand CPU_ISET_65SC02)
        stz     CH              ; Goto left edge of screen
        .else
        lda     #$00            ; Goto left edge of screen
        sta     CH
        .endif
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
        beq     putchardirect
        cmp     #$E0            ; Lowercase?
        bcc     mask
        and     #$7F            ; Inverse lowercase
        bra     putchardirect
        .endif
mask:   and     INVFLG          ; Apply normal, inverse, flash

putchardirect:
        pha
        .ifdef  __APPLE2ENH__
        lda     CH
        bit     RD80VID         ; In 80 column mode?
        bpl     put             ; No, just go ahead
        lsr                     ; Div by 2
        bcs     put             ; Odd cols go in main memory
        bit     HISCR           ; Assume SET80COL
put:    tay
        .else
        ldy     CH
        .endif
        lda     (BASL),Y        ; Get current character
        tax                     ; Return old character for _cgetc
        pla
        sta     (BASL),Y
        .ifdef  __APPLE2ENH__
        bit     LOWSCR          ; Doesn't hurt in 40 column mode
        .endif
        rts
