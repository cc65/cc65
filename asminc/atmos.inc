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

CAPSLOCK        := $20C
PATTERN         := $213
IRQVec          := $245
CURS_Y          := $268
CURS_X          := $269
STATUS          := $26A
TIMER3          := $276
CFOUND_NAME     := $293
KEYBUF          := $2DF
PARAM1          := $2E1        ; & $2E2
PARAM2          := $2E3        ; & $2E4
PARAM3          := $2E5        ; & $2E6


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
