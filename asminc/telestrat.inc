;
; Oric Telemon definition
; Telemon 2.4 & Telemon 3.0
; For telemon 3.0 check http://orix.oric.org
;


; ---------------------------------------------------------------------------
; Constants

SCREEN_XSIZE    = 40            ; screen columns
SCREEN_YSIZE    = 28            ; screen rows

FUNCTKEY        = $A5

FNAME_LEN       = 11            ; maximum length of file-name

FILENAME_MAX    = 100

; ---------------------------------------------------------------------------
; Zero page

; ---------------------------------------------------------------------------
; Page 00
RES             := $00
RESB            := $02

TR0             := $0C
TR1             := $0D
TR2             := $0E
TR3             := $0F
TR4             := $10
TR5             := $11
TR6             := $12
TR7             := $13




PTR_READ_DEST   := $2C           ; used for XFREAD and XWRITE only in telemon 3.0

HRSX            := $46
HRSY            := $47

HRS1            := $4D
HRS2            := $4F
HRS3            := $51
HRS4            := $53
HRS5            := $55



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
XRD0             = $08
XRDW0            = $0C
XWR0             = $10 
XWSTR0           = $14 
XTEXT            = $19
XHIRES           = $1A
XFILLM           = $1C
XMINMA           = $1F
XVARS            = $24          ; only in TELEMON 3.0, in telemon 2.4, it's XNOMFI ($24)
XFREAD           = $27          ; only in TELEMON 3.0
XOPEN            = $30          ; only in TELEMON 3.0
XCOSCR           = $34          ; switch off cursor
XCSSCR           = $35          ; switch on cursor
XCLOSE           = $3A          ; only in TELEMON 3.0 Close file
XFWRITE          = $3B          ; only in TELEMON 3.0 write file
XSONPS           = $40
XOUPS            = $42
XPLAY            = $43
XSOUND           = $44 
XMUSIC           = $45 
XZAP             = $46
XSHOOT           = $47
XCIRCL           = $8F
XCURSE           = $90
XPAPER           = $92
XINK             = $93
XEXPLO           = $9C 
XPING            = $9D

; ---------------------------------------------------------------------------
; ROM entries variables

PWD_PTR          = $00

; ---------------------------------------------------------------------------
; Page $200
SCRX             := $220
SCRY             := $224
ADSCRL           := $218
ADSCRH           := $21C


; ---------------------------------------------------------------------------
; Page $500

BUFNOM           := $517
BUFEDT           := $590

MAX_BUFEDT_LENGTH=110

; Hardware
CH376_DATA       := $340
CH376_COMMAND    := $341

; MACRO 

.macro  BRK_TELEMON   value
	.byte $00,value
.endmacro
