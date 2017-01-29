;
; Oric Telemon definition
; Telemon 2.4
;


; ---------------------------------------------------------------------------
; Constants

SCREEN_XSIZE    = 40            ; screen columns
SCREEN_YSIZE    = 28            ; screen rows

FUNCTKEY        = $A5

FNAME_LEN       = 11            ; maximum length of file-name


; ---------------------------------------------------------------------------
; Zero page




; ---------------------------------------------------------------------------
; Low memory




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

XWR0   := $10 
XWSTR0 := $14 

