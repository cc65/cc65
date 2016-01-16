; ca65 doesn't have a directive to set the high bit on text
; as the original author didn't know anything about the Apple. :-/
; http://www.cc65.org/mailarchive/2003-05/2992.html
;
; NOTE: There is no "standard" directive to set the high bit on ASCII text.
;
; * The assembler used in the redbook uses `msb on` and `msb off`
; * S-C Macro Assembler used
;     .AS for normal ASCII, and
;     .AT for normal ASCII but the last char has the high bit ON
; * Merlin uses yet another variation:
;     ASC 'Hello'   ; high bit is off
;     ASC "Hello"   ; high bit is ON
;   See the new Merlin-32 project
;   http://brutaldeluxe.fr/products/crossdevtools/merlin/
; 
; "The 'nice' thing about standards, is that there are so many to pick from!"
;
; NOTE: If you use the buggy '*' operator (see note below) this macro won't
; process the last 4 chars! Once we used a working "END-MAIN" we no longer
; need to manually set the high bit when printing characters via `ORA #80`.

; Force APPLE 'text' to have high bit on
.macro AS text
    .repeat .strlen(text), I
        .byte   .strat(text, I) | $80
    .endrep
.endmacro

; Force ASCII 'text' but last character has high bit on
; NOTE: There is no lowercase flashinng characters - they will appar as gibbersh!
.macro AT text
    .repeat .strlen(text)-1, I
        .byte   .strat(text, I) .BITAND $7F
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
.macro AC text
    .repeat .strlen(text), I
        .byte   .strat(text, I) & $1F
    .endrep
.endmacro

