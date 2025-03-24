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

        .include        "zeropage.inc"
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
        cmp     #$0D            ; Test for \r = carriage return
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
        .ifdef  __APPLE2ENH__
        bit     RD80VID         ; In 80 column mode?
        bpl     :+
        inc     OURCH           ; Bump to next column
        lda     OURCH
        bra     check           ; Must leave CH alone
:       .endif
        inc     CH              ; Bump to next column
        lda     CH
check:  cmp     WNDWDTH
        bcc     done
        jsr     newline
left:
        .ifdef  __APPLE2ENH__
        stz     CH              ; Goto left edge of screen
        bit     RD80VID         ; In 80 column mode?
        bpl     done
        stz     OURCH           ; Goto left edge of screen
        .else
        lda     #$00            ; Goto left edge of screen
        sta     CH
        .endif
done:   rts

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
        tax
        ldy     CH
        .ifdef  __APPLE2ENH__
        sec                     ; Assume main memory
        bit     RD80VID         ; In 80 column mode?
        bpl     put             ; No, just go ahead
        lda     OURCH
        lsr                     ; Div by 2
        tay
        bcs     put             ; Odd cols go in main memory
        php
        sei                     ; No valid MSLOT et al. in aux memory
        bit     HISCR           ; Assume SET80COL
        .endif
put:    lda     (BASL),Y        ; Get current character
        sta     tmp3            ; Save old character for _cgetc
        txa
        sta     (BASL),Y
        .ifdef  __APPLE2ENH__
        bcs     :+              ; In main memory
        bit     LOWSCR
        plp
:       .endif
        rts
