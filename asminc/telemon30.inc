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


.struct VIA2                     ; Versatile Interface Adapter
        .res    $0320
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

; primitives telemon 2.4 
XRD0   := $08
XRDW0  := $0c
XWR0   := $10 
XWSTR0 := $14 
XTEXT  := $19
XHIRES := $1A
XMINMA := $1f 
XFREAD := $27 ; only in TELEMON 3.0
XOPEN  := $30 ; only in TELEMON 3.0
XCOSCR := $34 ; switch off cursor
XCSSCR := $35 ; switch on cursor
XCLOSE := $3a ; Close file
XFWRITE:= $3b ; write

XSONPS := $40
XOUPS  := $42
XPLAY  := $43
XSOUND := $44 
XMUSIC := $45 
XZAP   := $46
XSHOOT := $47
XCIRCL := $8f
XCURSE := $90
XEXPLO := $9c 
XPING  := $9d
XPAPER := $92
XINK   := $93


; ---------------------------------------------------------------------------
; Page 00
RES  := $00
RESB := $02

TR0	 := $0c
TR1	 := $0d

PTR_READ_DEST := $2c ; used for XFREAD and XWRITE

HRSX := $46
HRSY := $47

HRS1 := $4D
HRS2 := $4F
HRS3 := $51
HRS4 := $53
HRS5 := $55

; ---------------------------------------------------------------------------
; Page $500

BUFNOM := $517
BUFEDT := $590

MAX_BUFEDT_LENGTH=110

; Hardware
CH376_DATA		:=$340
CH376_COMMAND	:=$341

; MACRO 


.macro  BRK_TELEMON   value
	.byte $00,value
.endmacro
