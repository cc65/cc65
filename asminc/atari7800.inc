; Atari 7800 TIA & RIOT read / write registers
;
; Karri Kaksonen (karri@sipo.fi), 2022

; TIA, RIOT & MARIA registers mapping
.include "atari7800_tia.inc"
.include "atari7800_riot.inc"
.include "atari7800_maria.inc"

; constants for the conio implementation
mono_charsperline    = 40
charsperline    = 20
screenrows      = 28

