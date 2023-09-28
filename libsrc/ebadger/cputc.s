;
; Ullrich von Bassewitz, 06.08.1998
;
; void __fastcall__ cputcxy (unsigned char x, unsigned char y, char c);
; void __fastcall__ cputc (char c);
;

        .export         _cputcxy, _cputc
        .export         cputdirect, newline, putchar, putchardirect
        .import         gotoxy, VTABZ

        .segment        "ONCE"

        .include "ebadger.inc"

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

cputdirect:
        jsr     putchar
        inc     CH              ; Bump to next column
        lda     CH
        cmp     WNDWDTH
        bcc     :+
        jsr     newline
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
mask:   ;and     INVFLG          ; Apply normal, inverse, flash

putchardirect:
        pha
        ldy     CH
put:    lda     (BASL),Y        ; Get current character
        tax                     ; Return old character for _cgetc
        pla
        sta     (BASL),Y
        rts
