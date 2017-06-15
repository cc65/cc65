;
; unsigned char __fastcall__ textcolor (unsigned char color);
; unsigned char __fastcall__ bgcolor (unsigned char color);
; unsigned char __fastcall__ bordercolor (unsigned char color);
;


        .export     _textcolor, _bgcolor, _bordercolor

        .include        "pce.inc"
        .include        "extzp.inc"

_textcolor:
        ldx     CHARCOLOR       ; get old value
        sta     CHARCOLOR       ; set new value
        txa
        rts

_bgcolor:
        ldx     BGCOLOR         ; get old value
        sta     BGCOLOR         ; set new value
        asl     a
        tay

        stz     VCE_ADDR_LO
        stz     VCE_ADDR_HI
        lda     colors,y
        sta     VCE_DATA_LO
        lda     colors+1,y
        sta     VCE_DATA_HI

        txa
        rts

_bordercolor:
        lda     #0
        tax
        rts

        .rodata
        .export colors

colors:
        ;       G      R      B
        .word ((0<<6)+(0<<3)+(0))       ; 0 black
        .word ((7<<6)+(7<<3)+(7))       ; 1 white
        .word ((0<<6)+(7<<3)+(0))       ; 2 red
        .word ((7<<6)+(0<<3)+(7))       ; 3 cyan
        .word ((0<<6)+(5<<3)+(7))       ; 4 violett
        .word ((7<<6)+(0<<3)+(0))       ; 5 green
        .word ((0<<6)+(0<<3)+(7))       ; 6 blue
        .word ((7<<6)+(7<<3)+(0))       ; 7 yellow
        .word ((5<<6)+(7<<3)+(0))       ; 8 orange
        .word ((3<<6)+(4<<3)+(3))       ; 9 brown
        .word ((4<<6)+(7<<3)+(4))       ; a light red
        .word ((3<<6)+(3<<3)+(3))       ; b dark grey
        .word ((4<<6)+(4<<3)+(4))       ; c middle grey
        .word ((7<<6)+(4<<3)+(4))       ; d light green
        .word ((4<<6)+(4<<3)+(7))       ; e light blue
        .word ((6<<6)+(6<<3)+(6))       ; f light gray

;-------------------------------------------------------------------------------
; force the init constructor to be imported

        .import initconio
conio_init      = initconio
