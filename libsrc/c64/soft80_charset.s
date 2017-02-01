;
; Groepaz/Hitmen, 12.10.2015
;
; character set for use with the soft80 implementations
;

; the format of the data follows the following layout:
;
; - to avoid unnecessary petscii->screencode conversions, the order of the
;   individual characters is different to the C64 ROM charset:
;   - $00 - $1f screencodes $60 - $7f (petscii codes $a0 - $bf)
;   - $20 - $3f screencodes $20 - $3f (petscii codes $20 - $3f)
;   - $40 - $5f screencodes $00 - $1f (petscii codes $40 - $5f)
;   - $60 - $7f screencodes $40 - $5f (petscii codes $60 - $7f)
; - only 128 characters are defined here, the soft80 implementation will invert
;   the graphics data for inverted display on the fly.
; - since the charset is 4 by 8 pixels, only the lower 4bit of each byte is
;   used. the upper bits have to be 0.
; - finally the lower 4bits are "inverted", ie a space character is represented
;   as $0f, $0f, $0f, $0f, $0f, $0f, $0f, $0f
;
; the graphics data is arranged differently to normal C64 charsets for speed,
; first comes the first row of all characters, then the second row in the next
; block, etc. like this:
;
; +000 ....xxxx ......xx ....xxxx ........
; +080 ....xxxx ......xx ....xxxx ....xxxx
; +100 ....xxxx ......xx ....xxxx ....xxxx
; +180 ....x..x ......xx ....xxxx ....xxxx
; +200 ....x..x ......xx ........ ....xxxx
; +280 ....xxxx ......xx ........ ....xxxx
; +300 ....xxxx ......xx ........ ....xxxx
; +380 ....xxxx ......xx ........ ....xxxx
; [...]
; +040 ....x.xx ....xxxx ....xxxx ....xxxx
; +0c0 .....x.x ....xxxx .....xxx ....xxxx
; +140 .......x ....x.xx .....xxx ....x..x
; +1c0 .......x ....xx.x ......xx .....xxx
; +240 .....xxx ....x..x .....x.x .....xxx
; +2c0 .....x.x .....x.x .....x.x .....xxx
; +340 ....x.xx ....x..x ......xx ....x..x
; +3c0 ....xxxx ....xxxx ....xxxx ....xxxx

        .export         soft80_charset

        .segment "ONCE"
soft80_charset:
        .byte $0f,$03,$0f,$00,$0f,$07,$05,$0e
        .byte $0f,$05,$0e,$0b,$0f,$0b,$0f,$0f
        .byte $0f,$0b,$0f,$0b,$07,$07,$0e,$00
        .byte $00,$0f,$0e,$0f,$0c,$0b,$03,$03
        .byte $0f,$0b,$05,$05,$0b,$05,$0b,$0b
        .byte $0d,$07,$0f,$0f,$0f,$0f,$0f,$0d
        .byte $0b,$0b,$0b,$0b,$05,$01,$0b,$01
        .byte $0b,$0b,$0f,$0f,$0d,$0f,$07,$0b
        .byte $0b,$0f,$0f,$0f,$0f,$0f,$0f,$0f
        .byte $0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f
        .byte $0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f
        .byte $0f,$0f,$0f,$09,$07,$03,$0b,$0f
        .byte $0f,$0b,$03,$0b,$03,$01,$01,$0b
        .byte $05,$01,$09,$05,$07,$05,$05,$0b
        .byte $03,$0b,$03,$0b,$01,$05,$05,$05
        .byte $05,$05,$01,$0b,$07,$0b,$0f,$0a

        .byte $0f,$03,$0f,$0f,$0f,$07,$05,$0e
        .byte $0f,$0a,$0e,$0b,$0f,$0b,$0f,$0f
        .byte $0f,$0b,$0f,$0b,$07,$07,$0e,$00
        .byte $00,$0f,$0e,$0f,$0c,$0b,$03,$03
        .byte $0f,$0b,$05,$05,$09,$05,$05,$0b
        .byte $0b,$0b,$05,$0b,$0f,$0f,$0f,$0d
        .byte $05,$0b,$05,$05,$05,$07,$05,$05
        .byte $05,$05,$0f,$0f,$0b,$0f,$0b,$05
        .byte $05,$0f,$07,$0f,$0d,$0f,$09,$0f
        .byte $07,$0b,$0d,$07,$03,$0f,$0f,$0f
        .byte $0f,$0f,$0f,$0f,$0b,$0f,$0f,$0f
        .byte $0f,$0f,$0f,$0b,$07,$0b,$0b,$0b
        .byte $0f,$0b,$05,$05,$05,$07,$07,$05
        .byte $05,$0b,$0d,$05,$07,$01,$01,$05
        .byte $05,$05,$05,$05,$0b,$05,$05,$05
        .byte $05,$05,$0d,$0b,$07,$0b,$0f,$0a

        .byte $0f,$03,$0f,$0f,$0f,$07,$0a,$0e
        .byte $0f,$05,$0e,$0b,$0f,$0b,$0f,$0f
        .byte $0f,$0b,$0f,$0b,$07,$07,$0e,$0f
        .byte $00,$0f,$0d,$0f,$0c,$0b,$03,$03
        .byte $0f,$0b,$05,$00,$07,$0d,$0b,$07
        .byte $0b,$0b,$0b,$0b,$0f,$0f,$0f,$0b
        .byte $01,$03,$0d,$0d,$05,$03,$07,$0d
        .byte $05,$05,$0b,$0b,$0b,$08,$0b,$0d
        .byte $01,$0b,$07,$09,$0d,$0b,$0b,$09
        .byte $07,$0f,$0f,$07,$0b,$05,$03,$0b
        .byte $03,$09,$03,$09,$01,$05,$05,$05
        .byte $05,$05,$01,$0b,$0b,$0b,$05,$0b
        .byte $0f,$05,$05,$07,$05,$07,$07,$07
        .byte $05,$0b,$0d,$03,$07,$01,$01,$05
        .byte $05,$05,$05,$07,$0b,$05,$05,$05
        .byte $0b,$05,$0b,$0b,$0b,$0b,$0a,$05

        .byte $09,$03,$0f,$0f,$0f,$07,$0a,$0e
        .byte $0f,$0a,$0e,$08,$0f,$08,$03,$0f
        .byte $08,$00,$00,$03,$07,$07,$0e,$0f
        .byte $0f,$0f,$05,$0f,$0c,$03,$03,$03
        .byte $0f,$0b,$0f,$05,$0b,$0b,$0b,$0f
        .byte $0b,$0b,$01,$01,$0f,$01,$0f,$0b
        .byte $05,$0b,$0b,$0b,$01,$0d,$03,$0b
        .byte $0b,$09,$0f,$0f,$07,$0f,$0d,$0b
        .byte $01,$0d,$03,$07,$09,$05,$01,$05
        .byte $03,$03,$0d,$05,$0b,$01,$05,$05
        .byte $05,$05,$05,$07,$0b,$05,$05,$05
        .byte $05,$05,$0d,$0b,$0b,$0b,$05,$00
        .byte $00,$01,$03,$07,$05,$03,$03,$01
        .byte $01,$0b,$0d,$03,$07,$05,$01,$05
        .byte $03,$05,$03,$0b,$0b,$05,$05,$01
        .byte $0b,$0b,$0b,$00,$0b,$0b,$05,$05

        .byte $09,$03,$00,$0f,$0f,$07,$05,$0e
        .byte $05,$05,$0e,$08,$0c,$08,$03,$0f
        .byte $08,$00,$00,$03,$07,$07,$0e,$0f
        .byte $0f,$0f,$03,$03,$0f,$03,$0f,$0c
        .byte $0f,$0f,$0f,$00,$0d,$07,$04,$0f
        .byte $0b,$0b,$0b,$0b,$0f,$0f,$0f,$0b
        .byte $05,$0b,$07,$0d,$0d,$0d,$05,$0b
        .byte $05,$0d,$0f,$0f,$0b,$08,$0b,$0b
        .byte $07,$09,$05,$07,$05,$01,$0b,$05
        .byte $05,$0b,$0d,$03,$0b,$01,$05,$05
        .byte $05,$05,$07,$0b,$0b,$05,$05,$01
        .byte $0b,$05,$0b,$0b,$0b,$0b,$0f,$00
        .byte $00,$05,$05,$07,$05,$07,$07,$05
        .byte $05,$0b,$0d,$03,$07,$05,$01,$05
        .byte $07,$05,$03,$0d,$0b,$05,$05,$01
        .byte $0b,$0b,$0b,$00,$07,$0b,$05,$0a

        .byte $0f,$03,$00,$0f,$0f,$07,$05,$0e
        .byte $05,$0a,$0e,$0b,$0c,$0f,$0b,$0f
        .byte $0b,$0f,$0b,$0b,$07,$07,$0e,$0f
        .byte $0f,$00,$03,$03,$0f,$0f,$0f,$0c
        .byte $0f,$0f,$0f,$05,$03,$05,$05,$0f
        .byte $0b,$0b,$05,$0b,$0b,$0f,$0b,$07
        .byte $05,$0b,$07,$05,$0d,$05,$05,$0b
        .byte $05,$05,$0b,$0b,$0b,$0f,$0b,$0f
        .byte $05,$05,$05,$07,$05,$07,$0b,$09
        .byte $05,$0b,$0d,$05,$0b,$05,$05,$05
        .byte $03,$09,$07,$0d,$0b,$05,$0b,$01
        .byte $05,$09,$07,$0b,$0d,$0b,$0f,$0b
        .byte $0f,$05,$05,$05,$05,$07,$07,$05
        .byte $05,$0b,$05,$05,$07,$05,$05,$05
        .byte $07,$0b,$05,$05,$0b,$05,$0b,$05
        .byte $05,$0b,$07,$0b,$07,$0b,$05,$0a

        .byte $0f,$03,$00,$0f,$0f,$07,$0a,$0e
        .byte $0a,$05,$0e,$0b,$0c,$0f,$0b,$00
        .byte $0b,$0f,$0b,$0b,$07,$07,$0e,$0f
        .byte $0f,$00,$07,$03,$0f,$0f,$0f,$0c
        .byte $0f,$0b,$0f,$05,$0b,$05,$08,$0f
        .byte $0d,$07,$0f,$0f,$0b,$0f,$0b,$07
        .byte $0b,$01,$01,$0b,$0d,$0b,$0b,$0b
        .byte $0b,$0b,$0f,$0b,$0d,$0f,$07,$0b
        .byte $0b,$09,$03,$09,$09,$09,$0b,$0d
        .byte $05,$01,$0d,$05,$01,$05,$05,$0b
        .byte $07,$0d,$07,$03,$0d,$09,$0b,$05
        .byte $05,$0d,$01,$09,$0d,$03,$0f,$0b
        .byte $0f,$05,$03,$0b,$03,$01,$07,$0b
        .byte $05,$01,$0b,$05,$01,$05,$05,$0b
        .byte $07,$0d,$05,$0b,$0b,$0b,$0b,$05
        .byte $05,$0b,$01,$0b,$0b,$0b,$05,$05

        .byte $0f,$03,$00,$0f,$00,$07,$0a,$0e
        .byte $0a,$0a,$0e,$0b,$0c,$0f,$0b,$00
        .byte $0b,$0f,$0b,$0b,$07,$07,$0e,$0f
        .byte $0f,$00,$0f,$03,$0f,$0f,$0f,$0c
        .byte $0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f
        .byte $0f,$0f,$0f,$0f,$07,$0f,$0f,$0f
        .byte $0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f
        .byte $0f,$0f,$0f,$07,$0f,$0f,$0f,$0f
        .byte $0f,$0f,$0f,$0f,$0f,$0f,$0f,$03
        .byte $0f,$0f,$03,$0f,$0f,$0f,$0f,$0f
        .byte $07,$0d,$0f,$0f,$0f,$0f,$0f,$0f
        .byte $0f,$03,$0f,$0f,$0f,$0f,$0f,$0f
        .byte $0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f
        .byte $0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f
        .byte $0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f
        .byte $0f,$0f,$0f,$0b,$0b,$0b,$0f,$05
