;
; Oleg A. Odintsov, Moscow, 2024
;
; void __fastcall__ cputcxy (unsigned char x, unsigned char y, char c);
; void __fastcall__ cputc (char c);
;

    .import     COUT
    .export     _cputcxy, _cputc
    .import     gotoxy, VTABZ
    .include    "agat.inc"

_cputcxy:
    pha
    jsr    gotoxy
    pla
_cputc:
    cmp    #$0D
    bne    notleft
    ldy    #$00
    sty    CH
    rts
notleft:cmp    #$0A
    beq    newline
putchar:
    ldy    CH
    sta    (BASL),Y
    iny
    lda    TATTR
    bmi    wch
    sta    (BASL),Y
    iny
wch:sty    CH
    cpy    WNDWDTH
    bcc    noend
    ldy    #$00
    sty    CH
newline:inc    CV
    lda    CV
    cmp    WNDBTM
    bcc    :+
    lda    WNDTOP
    sta    CV
:   jmp    VTABZ
noend:    rts
