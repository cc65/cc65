;
; Oric TELEMON definition
; TELEMON 2.4 & Orix
; For Orix check http://orix.oric.org
;

; ---------------------------------------------------------------------------
; Constants

SCREEN_XSIZE    = 40            ; Screen columns
SCREEN_YSIZE    = 28            ; Screen rows

FUNCTKEY        = $A5

FNAME_LEN       = 11            ; Maximum length of file-name

; ---------------------------------------------------------------------------
; I/O Identifier
; Theses identifers are used for channel management
;

XKBD            = $80           ; Keyboard
XRSE            = $83           ; RS232 in
XSCR            = $88           ; Screen
XRSS            = $90           ; RS232 out

; ---------------------------------------------------------------------------
; Zero page

; ---------------------------------------------------------------------------
; Page 00
RES             := $00
RESB            := $02

DECDEB          := $04
DECFIN          := $06
DECCIB          := $08
DECTRV          := $0A

TR0             := $0C
TR1             := $0D
TR2             := $0E
TR3             := $0F
TR4             := $10
TR5             := $11
TR6             := $12
TR7             := $13

DEFAFF          := $14

IRQSVA          := $21          ; Used to save A when a BRK call occurs
IRQSVX          := $22          ; Used to save X when a BRK call occurs
IRQSVY          := $23          ; Used to save Y when a BRK call occurs
IRQSVP          := $24          ; Used to save P when a BRK call occurs

ADSCR           := $26
SCRNB           := $28          ; Id of the current window

ADKBD           := $2A          ; Address ASCII conversion table


PTR_READ_DEST   := $2C          ; Used for XFREAD and XWRITE only in Orix

ADCLK           := $40          ; Address for clock display
TIMEUS          := $42
TIMEUD          := $44          ; Counter clock (1/10 of a second)


HRSX            := $46
HRSY            := $47

XLPRBI          := $48          ; Printer flag (b7)

HRSX40          := $49
HRSX6           := $4A

ADHRS           := $4B          ; Hires screen address (word)

HRS1            := $4D
HRS2            := $4F
HRS3            := $51
HRS4            := $53
HRS5            := $55

HRSFB           := $57

VABKP1          := $58

; RS232T
; b0-b3 : speed
;         1111 => 19200 bps  (please note that telestrat can't handle this speed without stopping all IRQ except ACIA's one)
;         1100 =>  9600 bps  (default from TELEMON)
;         1110 =>  4800 bps
;         1010 =>  2400 bps
;         1000 =>  1200 bps
;         0111 =>   600 bps
;         0110 =>   300 bps
;         0101 =>   150 bps
;         0010 =>    75 bps

; b4    : 0 external clock, 1 internal clock
; b6-b5 : 00 8 bits
;         01 7 bits
;         10 6 bits
;         11 5 bits
; b7    : 0  a stop

RS232T          := $59

; RS232C
; b0-b3 : 0
; b4    : 1 if echo
; b5    : 1 if parity
; b7-b6 : 00 in/out parity odd
;       : 01 on/out parity even
;       : 10 parity sent, answer not tested
;       : 11 SPACE SENT, reception not tested

RS232C          := $5A
INDRS           := $5B

; Float and integer management
ACC1E           := $60
ACC1M           := $61
ACC1S           := $65
ACC1EX          := $66
ACC1J           := $67
ACC2E           := $68
ACC2M           := $69
ACC2S           := $6D
ACCPS           := $6E
ACC3            := $6F

ACC4E           := $73
ACC4M           := $74


FLDT0           := $74
FLDT1           := $75
FLDT2           := $76
FLSVY           := $77
FLTR0           := $7D
FLTR1           := $7E

; Menu management
MENDDY          := $62
MENDFY          := $63
MENX            := $64
MENDY           := $66
FLGMEN          := $68
ADMEN           := $69

FLSGN           := $8A
FLINT           := $88
FLSVS           := $89
FLERR           := $8B

VARLNG          := $8C
VARAPL          := $D0

; ---------------------------------------------------------------------------
; Low memory
IRQVec          := $02FB        ; "fast" interrupt vector

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


.struct VIA2                    ; Versatile Interface Adapter
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

; TELEMON primitives (2.4 & 3.x)

; all values are used to call bank 7 of telestrat cardridge. It works with 'brk value'
XOP0             = $00          ; Open device on channel 0
XOP1             = $01          ; Open device on channel 1
XOP2             = $02          ; Open device on channel 2
XOP3             = $03          ; Open device on channel 3

XCL0             = $04          ; Close channel 0
XCL1             = $05          ; Close channel 1
XCL2             = $06          ; Close channel 2
XCL3             = $07          ; Close channel 3

XRD0             = $08
XRDW0            = $0C

XWR0             = $10          ; Write a char in channel 0
XWR1             = $11          ; Write a char in channel 1
XWR2             = $12          ; Write a char in channel 2
XWR3             = $13          ; Write a char in channel 3

XWSTR0           = $14          ; Write a string in text mode channel 0
XWSTR1           = $15          ; Write a string in text mode channel 1
XWSTR2           = $16          ; Write a string in text mode channel 2
XWSTR3           = $17          ; Write a string in text mode channel 3

XDECAL           = $18

XTEXT            = $19
XHIRES           = $1A
XEFFHI           = $1B          ; Clear hires screen
XFILLM           = $1C
XMINMA           = $1F
XVARS            = $24          ; Only in Orix, in TELEMON 2.4, it's XNOMFI ($24)
XCRLF            = $25          ; Jump a line and return to the beginning of the line
XDECAY           = $26
XFREAD           = $27          ; Only in Orix
XBINDX           = $28          ; Convert a number into hex and displays on channel 0
XDECIM           = $29
XHEXA            = $2A          ; Convert a number into hex

XMAINARGS        = $2C          ; Only available for Orix

XEDT             = $2D          ; Launch editor
XINSER           = $2E
XGETARGV         = $2E          ; Only available for Orix

XSCELG           = $2F          ; Search a line in editor mode
XOPEN            = $30          ; Only in Orix
XECRPR           = $33          ; Displays prompt
XCOSCR           = $34          ; Switch off cursor
XCSSCR           = $35          ; Switch on cursor
XSCRSE           = $36
XSCROH           = $37          ; Scroll up text screen
XSCROB           = $38          ; Scroll down text screen
XSCRNE           = $39          ; Load charset from rom to ram
XCLOSE           = $3A          ; Only in Orix close file
XFWRITE          = $3B          ; Only in Orix write file

; Clock primitive
XRECLK           = $3C          ; Reset clock
XCLCL            = $3D          ; Close clock
XWRCLK           = $3E          ; Displays clock in the address in A & Y registers

XFSEEK           = $3F          ; Only in Orix

; Sound primitives
XSONPS           = $40          ; Send data to PSG register (14 values)
XOUPS            = $42          ; Send Oups sound into PSG
XPLAY            = $43          ; Play a sound
XSOUND           = $44
XMUSIC           = $45
XZAP             = $46          ; Send Zap sound to PSG
XSHOOT           = $47

; Path Management
XGETCWD          = $48          ; Get current CWD
XPUTCWD          = $49          ; Chdir

; File management
XMKDIR           = $4B          ; Create a folder. Only available in Orix

XHCHRS           = $4C          ; Hard copy hires

; File management
XRM              = $4D          ; Remove a folder or a file. Only available in Orix

XFWR             = $4E          ; Put a char on the first screen. Only available in Orix

; Keyboard primitives
XALLKB           = $50          ; Read Keyboard, and populate KBDCOL
XKBDAS           = $51          ; Ascii conversion
XGOKBD           = $52          ; Swap keyboard type (Qwerty, French ...)

; Buffer management
XECRBU           = $54          ; Write A or AY in the buffer
XLISBU           = $55          ; Read A or AY in the buffer
XTSTBU           = $56
XVIDBU           = $57          ; Flush the buffer
XINIBU           = $58          ; Initialize the buffer X
XDEFBU           = $59          ; Reset all value of the buffer
XBUSY            = $5A          ; Test if the buffer is empty

XMALLOC          = $5B          ; Only in Orix

; RS232 primitives
XSDUMP           = $5C          ; RS232 input dump
XCONSO           = $5D          ; Swap screen into RS232 terminal
XSLOAD           = $5E          ; Read a file from RS232
XSSAVE           = $5F          ; Write a file to RS232

; Minitel primitives
XMLOAD           = $60          ; Read a file from Minitel
XMSAVE           = $61          ; Write a file to Minitel

XFREE            = $62          ; Only in Orix

XEXEC            = $63          ; Only in Orix

; Next Minitel primitives
XWCXFI           = $63          ; Wait connection
XLIGNE           = $64          ;
XDECON           = $65          ; Minitel disconnection
XMOUT            = $66          ; Send a byte to minitel (from A)

XSOUT            = $67          ; Send accumulator value (A) to RS232, available in TELEMON 2.4 : if RS232 buffer is full, the Oric Telestrat freezes

XHRSSE           = $8C          ; Set hires position cursor
XDRAWA           = $8D          ; Draw a line absolute
XDRAWR           = $8E          ; Draw a line (relative)
XCIRCL           = $8F          ; Draw a circle
XCURSE           = $90          ; Plot a pixel
XCURMO           = $91          ; Move to x,y pos in Hires
XPAPER           = $92
XINK             = $93
XBOX             = $94          ; Draw a box
XABOX            = $95
XFILL            = $96
XCHAR            = $97          ; Display a char on the screen in Hires
XSCHAR           = $98          ; Draw a string in hires
XEXPLO           = $9C          ; Send Explode sound to PSG
XPING            = $9D          ; Send Ping sound to PSG

; ---------------------------------------------------------------------------
; ROM entries variables

PWD_PTR          = $00

; ---------------------------------------------------------------------------
;
BUFTRV           := $100


; ---------------------------------------------------------------------------
; Page $200
BNKST            := $200        ; Used to store signature of 8 bank (length : 8 bytes)
TABDRV           := $208

DRVDEF           := $20C
FLGTEL           := $20D
KOROM            := $20E        ; Used to compute the size of all rom bank. The result is store here. The value is in KB
KORAM            := $20F        ; Used to compute the size of all ram bank. The result is store here. The value is in KB
; Time management
TIMED            := $210        ; Clock (1/10 of seconds)
TIMES            := $211
TIMEM            := $212
TIMEH            := $213
FLGCLK           := $214
FLGCLK_FLAG      := $215
FLGCUR           := $216        ; Cursor management flag
; screens position managements

FLGCUR_STATE     := $217        ; Cursor state flag

ADSCRL           := $218
ADSCRH           := $21C
SCRX             := $220
SCRY             := $224

SCRDX            := $228
SCRFX            := $22C
SCRDY            := $230
SCRFY            := $234
SCRBAL           := $238
SCRBAH           := $23C
SCRCT            := $240
SCRCF            := $244
FLGSCR           := $248
CURSCR           := $24C

HARD_COPY_HIRES  := $250        ; Hard copy vector

SCRTXT           := $256
SCRHIR           := $25C
SCRTRA           := $262        ; 6 bytes lenfth

; Keyboard management
KBDCOL           := $268        ; 8 bytes length
KBDFLG_KEY       := $270        ; 0 if no key pressed
KBDVRR           := $272

KBDVRL           := $273
FLGKBD           := $275
KBDFCT           := $276
KBDSHT           := $278

KBDKEY           := $279
KBDCTC           := $27E
LPRX             := $286
LPRY             := $287
LPRFX            := $288
LPRFY            := $289
FLGLPR           := $28A

; Joysticks management
FLGJCK           := $28C
JCGVAL           := $28D
JCDVAL           := $28E
JCKTAB           := $29D


HRSPAT           := $2AA        ; Hires pattern : it's used to draw pattern for a line or a circle
HRSERR           := $2AB

IOTAB0           := $2AE
IOTAB1           := $2B2
IOTAB2           := $2B6
IOTAB3           := $2BA
ADIOB            := $2BE        ; 48 bytes length
FLGRST           := $2EE
CSRND            := $2EF
VNMI             := $2F4
ADIODB_VECTOR    := $2f7        ; 3 bytes length

IRQVECTOR        := $2FA
VAPLIC           := $2FD

; ---------------------------------------------------------------------------
; Page $400
EXBNK            := $40C
VEXBNK           := $414
BNKCIB           := $417

; ---------------------------------------------------------------------------
; Page $500

DRIVE            := $500
ERRNB            := $512
SAVES            := $513
BUFNOM           := $517
VSALO0           := $528
VSALO1           := $529
FTYPE            := $52C        ; File type
DESALO           := $52D
FISALO           := $52F
EXSALO           := $531
EXTDEF           := $55D        ; Default extension. At the start of telemon, it's set to ".COM"
BUFEDT           := $590        ; Buffer edition

MAX_BUFEDT_LENGTH=110

; ---------------------------------------------------------------------------
; Hardware
CH376_DATA       := $340
CH376_COMMAND    := $341

; RAM overlays buffer
BUFBUF           := $c080

; ---------------------------------------------------------------------------
; Stratsed vectors
; Stratsed is the main OS for Telestrat
XMERGE           := $FF0E
XFST             := $FF11
XSPUT            := $FF14
XSTAKE           := $FF17
XTAKE            := $FF20
XOPENS           := $FF1A       ; XOPEN from Stratsed
XCLOSES          := $FF1D       ; XCLOSE from Stratsed
XPUT             := $FF23
XREWIN           := $FF29
XJUMP            := $FF2C
XLGBUF           := $FF2F
XERVEC           := $FF32
XESAVE           := $FF35
XCOPY            := $FF38
XDNAME           := $FF3B
XSTATU           := $FF3E
XUPDAT           := $FF41
XFORMA           := $FF44
XDELBK           := $FF4A
XDELN            := $FF4D
XPROT            := $FF50
XUNPRO           := $FF53
XDIRN            := $FF56
XBKP             := $FF59
XINITI           := $FF5C
XERREU           := $FF5F
XLOAD            := $FF62
XDEFSA           := $FF65
XDEFLO           := $FF68
XSAVE            := $FF6B
XNOMDE           := $FF6E
XCREAY           := $FF71
XDETSE           := $FF74
XLIBSE           := $FF77
XTRVCA           := $FF7A
XTRVNM           := $FF7D
XTRVNX           := $FF80
XBUCA            := $FF86
XVBUF1           := $FF89
XSVSEC           := $FF8C
XSAY             := $FF8F
XSBUF1           := $FF92
XSBUF2           := $FF95
XSBUF3           := $FF98
XSCAT            := $FF9B
XPRSEC           := $FFA1
XPBUF1           := $FFA4
XPMAP            := $FFA7
XRWTS            := $FFAA

; ---------------------------------------------------------------------------
; MACRO

.macro  BRK_TELEMON   value
        .byte $00,value
.endmacro
