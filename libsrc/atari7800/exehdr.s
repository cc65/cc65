;
; Karri Kaksonen, 2022
;
; This header contains data for emulators
;
        .export         __EXEHDR__: absolute = 1

; ------------------------------------------------------------------------
; EXE header
	.segment "EXEHDR"
	.byte	3					; version
	.byte	'A','T','A','R','I','7','8','0','0',' ',' ',' ',' ',' ',' ',' '
	.byte	'G','a','m','e',' ','n','a','m','e',0,0,0,0,0,0,0
	.byte	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	.byte	0,0,$80,0					; Size
    ;    bit  0 - pokey at 4000
    ;    bit  1 - supergame bank switched
    ;    bit  2 - supergame ram at $4000
    ;    bit  3 - rom at $4000
    ;    bit  4 - bank 6 at $4000
    ;    bit  5 - supergame banked ram
    ;    bit  6 - pokey at $450
    ;    bit  7 - mirror ram at $4000
    ;    bit  8 - activision banking
    ;    bit  9 - absolute banking
    ;    bit 10 - pokey at $440
    ;    bit 11 - ym2151 at $461/462
    ;    bit 12 - souper
    ;    bit 13-15 - Special
    ;   0 = Normal cart
	.byte	0,0					; 0 = Normal cart
	.byte	1					; 1 = Joystick, 2 = lightgun
	.byte	0					; No joystick 2
        .byte	0					; bit0 = 0:NTSC,1:PAL bit1 = 0:component,1:composite
        .byte   0  ; Save data peripheral - 1 byte (version 2)
    ;    0 = None / unknown (default)
    ;    1 = High Score Cart (HSC)
    ;    2 = SaveKey

        .byte	0					; 63   Expansion module
    ;    0 = No expansion module (default on all currently released games)
    ;    1 = Expansion module required
	.byte	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	.byte	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	.byte	0,0,0,0,0,0,0,0
        .byte	'A','C','T','U','A','L',' ','C','A','R','T',' ','D','A','T','A',' ','S','T','A','R','T','S',' ','H','E','R','E'
