;
; Ullrich von Bassewitz, 06.08.1998
;
; void __fastcall__ cputcxy (unsigned char x, unsigned char y, char c);
; void __fastcall__ cputc (char c);
;

        ; Call constructor *after* the 80-columns card detection
        ; in videomode.s
        .constructor    initconio, 7
        .export         _cputcxy, _cputc
        .export         cputdirect, newline, putchar, putchardirect
        .import         gotoxy, VTABZ

        .ifndef __APPLE2ENH__
        .import         iie_or_newer
        .import         uppercasemask
        .endif

        .include        "zeropage.inc"
        .include        "apple2.inc"

        .macpack        cpu

        .segment        "ONCE"

initconio:
        .ifndef __APPLE2ENH__
        bit     iie_or_newer
        bmi     :+
        rts
:
        .endif
        sta     SETALTCHAR      ; Switch in alternate charset
        bit     LORES           ; Limit SET80COL-HISCR to text
        rts

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

        .ifndef __APPLE2ENH__
        bit     iie_or_newer
        bpl     :+
        .endif
        bit     RD80VID         ; In 80 column mode?
        bpl     :+
        inc     OURCH           ; Bump to next column
        lda     OURCH
        .ifdef __APPLE2ENH__
        bra     check           ; Must leave CH alone
        .else
        jmp     check
        .endif

:       inc     CH              ; Bump to next column
        lda     CH
check:  cmp     WNDWDTH
        bcc     done
        jsr     newline
left:
        .ifdef  __APPLE2ENH__
        stz     CH              ; Goto left edge of screen
        .else
        lda     #$00
        sta     CH
        .endif

        .ifndef __APPLE2ENH__
        bit     iie_or_newer
        bpl     done
        .endif

        bit     RD80VID         ; In 80 column mode?
        bpl     done
        .ifdef  __APPLE2ENH__
        stz     OURCH           ; Goto left edge of screen
        .else
        sta     OURCH
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

        sec                     ; Assume main memory

        .ifndef __APPLE2ENH__
        bit     iie_or_newer
        bpl     put
        .endif

        bit     RD80VID         ; In 80 column mode?
        bpl     put             ; No, just go ahead
        lda     OURCH
        lsr                     ; Div by 2
        tay
        bcs     put             ; Odd cols go in main memory
        php
        sei                     ; No valid MSLOT et al. in aux memory
        bit     HISCR           ; Assume SET80COL

put:    lda     (BASL),Y        ; Get current character
        sta     tmp3            ; Save old character for _cgetc
        txa
        sta     (BASL),Y

        bcs     :+              ; In main memory
        bit     LOWSCR
        plp
:       rts
