; ca65 doesn't have a directive to control (set, clear, toggle) the high bit on
; text as, sadly, the original author didn't know anything about the Apple. :-(
;
; * http://www.cc65.org/mailarchive/2003-05/2992.html
; * http://www.cc65.org/mailarchive/2003-05/2995.html
;
;    Can someone explain me how characters work in the apple? This is the first
;    time I hear something about the usage of bit 7.
;
; **NOTE**: If you are familiar with other Apple 2 assemblers:
;
;     There is no "standard" directive to set the high bit on ASCII text.
;
; * The assembler used in the redbook uses `msb on` and `msb off`
; * S-C Macro Assembler used
;     .AS for normal ASCII, and
;     .AT for normal ASCII but the last char has the high bit ON
; * Merlin uses yet another variation:
;     ASC 'Hello'   ; high bit is off
;     ASC "Hello"   ; high bit is ON
;   See the new Merlin-32 project at:
;   * http://brutaldeluxe.fr/products/crossdevtools/merlin/
; 
; "The 'nice' thing about standards, is that there are so many to pick from!"
;
; Force APPLE 'text' to have high bit on
.macro AS text
    .repeat .strlen(text), I
        .byte   .strat(text, I) | $80
    .endrep
.endmacro

; Force ASCII 'text' but last character has high bit on
; Will display as FLASHING characters
; NOTE: There is no lowercase flashinng characters - they will appar as gibbersh!
.macro AT text
    .repeat .strlen(text)-1, I
        .byte   .strat(text, I) & $7F
    .endrep
    .byte   .strat(text, .strlen(text)-1) | $80
.endmacro

; Force APPLE 'text' with high bit on but last character has high bit off
.macro AR text
    .repeat .strlen(text)-1, I
        .byte   .strat(text, I) | $80
    .endrep
    .byte   .strat(text, .strlen(text)-1) & $7F
.endmacro

; Force ASCII 'text' to be control chars: $00..$1F
; Will display as INVERSE characters
.macro AC text
    .repeat .strlen(text), I
        .byte   .strat(text, I) & $1F
    .endrep
.endmacro

