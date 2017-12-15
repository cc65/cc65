;
; Oric Atmos definitions
; BASIC 1.1 addresses
;


; ---------------------------------------------------------------------------
; Constants

SCREEN_XSIZE    = 40            ; screen columns
SCREEN_YSIZE    = 28            ; screen rows

FUNCTKEY        = $A5

FNAME_LEN       = 16            ; maximum length of file-name


; ---------------------------------------------------------------------------
; Zero page

SCRPTR          := $12
BASIC_BUF       := $35
CHARGOT         := $E8
TXTPTR          := $E9


; ---------------------------------------------------------------------------
; Low memory

MODEKEY         := $0209
CAPSLOCK        := $020C        ; $7F = not locked, $FF = locked
PATTERN         := $0213
IRQVec          := $0245        ; "fast" interrupt vector
JOINFLAG        := $025A        ; 0 = don't joiu, $4A = join BASIC programs
VERIFYFLAG      := $025B        ; 0 = load, 1 = verify
CURS_Y          := $0268
CURS_X          := $0269
STATUS          := $026A
BACKGRND        := $026B
FOREGRND        := $026C
TIMER3          := $0276
CFILE_NAME      := $027F
CFOUND_NAME     := $0293
FILESTART       := $02A9
FILEEND         := $02AB
AUTORUN         := $02AD        ; $00 = only load, $C7 = autorun
LANGFLAG        := $02AE        ; $00 = BASIC, $80 = machine code
LOADERR         := $02B1
KEYBUF          := $02DF
PARMERR         := $02E0
PARAM1          := $02E1        ; & $02E2
PARAM2          := $02E3        ; & $02E4
PARAM3          := $02E5        ; & $02E6
BANGVEC         := $02F5


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

GETLINE         := $C592
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

; Sound Effects
PING            := $FA9F
PING1           := $FA85
SHOOT           := $FAB5
SHOOT1          := $FA9B
EXPLODE         := $FACB
EXPLODE1        := $FAB1
ZAP             := $FAE1
ZAP1            := $FAC7
TICK            := $FB14
TICK1           := $FAFA
TOCK            := $FB2A
TOCK1           := $FB10
