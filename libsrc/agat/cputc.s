;
; Oleg A. Odintsov, Moscow, 2024
; Konstantin Fedorov, 12.06.2025
;
; void __fastcall__ cputcxy (unsigned char x, unsigned char y, char c);
; void __fastcall__ cputc (char c);
;

    .import     COUT
    .export     _cputcxy, _cputc, newline, putchar,putchardirect
    .import     gotoxy, VTABZ
    .include    "agat.inc"

_cputcxy:
    pha
    jsr    gotoxy
    pla
_cputc:
    cmp    #$0D         ; Test for \r = carriage return
    bne    notleft
    ldy    #$00
    sty    CH
    rts
notleft:
    cmp    #$0A         ; Test for \n = line feed
    beq    newline

putchar:
    ldy    CH
    sta    (BASL),Y
    iny
    lda    TATTR
    bmi    wch          ; Skip if t64
    sta    (BASL),Y
    iny
wch:
    sty    CH
    cpy    WNDWDTH
    bcc    noend
    ldy    #$00
    sty    CH
newline:
    inc    CV
    lda    CV
    cmp    WNDBTM
    bcc    :+
    lda    WNDTOP
    sta    CV
:   jmp    VTABZ
noend:
    rts

putchardirect:
    ldy    CH
    sta    (BASL),Y
    lda    TATTR
    bmi    :+
    iny
    sta    (BASL),Y
:   rts
