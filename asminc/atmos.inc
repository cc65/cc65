;
; Oric Atmos definitions
; BASIC 1.1 addresses
;


; ---------------------------------------------------------------------------
; Constants

FNAME_LEN       = 16            ; maximum length of file-name


; ---------------------------------------------------------------------------
; Zero page

BASIC_BUF       := $35


; ---------------------------------------------------------------------------
; Low memory

CAPSLOCK        := $020C
PATTERN         := $0213
IRQVec          := $0245
CURS_Y          := $0268
CURS_X          := $0269
STATUS          := $026A
TIMER3          := $0276
CFOUND_NAME     := $0293
KEYBUF          := $02DF
PARAM1          := $02E1        ; & $02E2
PARAM2          := $02E3        ; & $02E4
PARAM3          := $02E5        ; & $02E6


; ---------------------------------------------------------------------------
; I/O locations

; 6522
.struct VIA                     ; Versatile Interface Adapter
        .res    $0300
PRB     .byte                   ; Port Register B
PRA     .byte                   ; Port Register A
DDRB    .byte                   ; Data Direction Register B
DDRA    .byte                   ; Data Direction Register A
T1      .word                   ; Timer 1
T1L     .word                   ; Timer 1 Latch
T2      .word                   ; Timer 2
SR      .byte                   ; Shift Register
ACR     .byte                   ; Auxiliary Control Register
PCR     .byte                   ; Peripheral Control Register
IFR     .byte                   ; Interrupt Flags Register
IER     .byte                   ; Interrupt Enable Register
PRA2    .byte                   ; Port Register A without handshaking
.endstruct

; 6551
.struct ACIA                    ; Asynchronous Communications Interface Adapter
        .res    $031C
DATA    .byte
STATUS  .byte
CMD     .byte                   ; Command register
CTRL    .byte                   ; Control register
.endstruct

SCREEN          := $BB80


; ---------------------------------------------------------------------------
; ROM entries

TEXT            := $EC21
HIRES           := $EC33
CURSET          := $F0C8
CURMOV          := $F0FD
DRAW            := $F110
CHAR            := $F12D
POINT           := $F1C8
PAPER           := $F204
INK             := $F210
PRINT           := $F77C
