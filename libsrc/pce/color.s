;
; unsigned char __fastcall__ textcolor (unsigned char color);
; unsigned char __fastcall__ bgcolor (unsigned char color);
; unsigned char __fastcall__ bordercolor (unsigned char color);
;

        .export         _textcolor, _bgcolor, _bordercolor
        .export         colors

        .import         return0

        .include        "pce.inc"
        .include        "extzp.inc"

_bordercolor    := return0              ; always black

_textcolor:
        ldx     CHARCOLOR               ; get old value
        sta     CHARCOLOR               ; set new value
        txa
        rts

_bgcolor:
        and     #$0F
        ldx     BGCOLOR                 ; get old value
        sta     BGCOLOR                 ; set new value
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

.rodata

        ;         G      R      B
colors: .word   ((0<<6)+(0<<3)+(0))     ; $0 black
        .word   ((7<<6)+(7<<3)+(7))     ; $1 white
        .word   ((0<<6)+(7<<3)+(0))     ; $2 red
        .word   ((7<<6)+(0<<3)+(7))     ; $3 cyan
        .word   ((0<<6)+(5<<3)+(7))     ; $4 violet
        .word   ((7<<6)+(0<<3)+(0))     ; $5 green
        .word   ((0<<6)+(0<<3)+(7))     ; $6 blue
        .word   ((7<<6)+(7<<3)+(0))     ; $7 yellow
        .word   ((5<<6)+(7<<3)+(0))     ; $8 orange
        .word   ((3<<6)+(4<<3)+(3))     ; $9 brown
        .word   ((4<<6)+(7<<3)+(4))     ; $A light red
        .word   ((3<<6)+(3<<3)+(3))     ; $B dark grey
        .word   ((4<<6)+(4<<3)+(4))     ; $C middle grey
        .word   ((7<<6)+(4<<3)+(4))     ; $D light green
        .word   ((4<<6)+(4<<3)+(7))     ; $E light blue
        .word   ((6<<6)+(6<<3)+(6))     ; $F light gray

;-------------------------------------------------------------------------------
; force the init constructor to be imported

        .import initconio
conio_init      = initconio
