;
; CX16 r39 definitions
;

; ---------------------------------------------------------------------------
; Constants

.enum COLOR
  BLACK         = $00
  WHITE
  RED
  CYAN
  PURPLE
  GREEN
  BLUE
  YELLOW
  ORANGE
  BROWN
  PINK
  LIGHTRED      = PINK
  GRAY1
  GRAY2
  LIGHTGREEN
  LIGHTBLUE
  GRAY3
.endenum

; Special characters
.enum CH
COLOR_SWAP      = $01
STOP            = $03
UNDERLINE
WHITE
BOLD
BELL
BACKSPACE
TAB
LINEFEED
ITALIC
OUTLINE
ENTER
FONT_LOWER
FONT_ISO
F9
CURS_DOWN
REVERSE
HOME
DEL
F10
F11
F12
SHIFT_TAB
RED             = $1C
CURS_RIGHT
GREEN
BLUE
LIRA            = $5C
ORANGE          = $81
RUN             = $83
HELP
F1
F3
F5
F7
F2
F4
F6
F8
SHIFT_ENTER
FONT_UPPER
FONT_PET
BLACK
CURS_UP
ATTR_CLEAR
SCRN_CLEAR
INS
BROWN
PINK
LIGHTRED        = PINK
GRAY1
GRAY2
LIGHTGREEN
LIGHTBLUE
GRAY3
PURPLE
CURS_LEFT
YELLOW
CYAN
SHIFT_SPACE
LTEE            = $AB
LLCORNER        = $AD
URCORNER
ULCORNER        = $B0
BTEE
TTEE
RTEE
LRCORNER        = $BD
HLINE           = $C0
CROSS           = $DB
VLINE           = $DD
PI
.endenum

; ---------------------------------------------------------------------------
; Zero page

; Banking registers
RAM_BANK        := $00
ROM_BANK        := $01

; GEOS and graphics pseudo-registers
.struct gREG
                .org    $02
  .union
  r0            .word
  .struct
  r0L           .byte
  r0H           .byte
  .endstruct
  .endunion
  .union
  r1            .word
  .struct
  r1L           .byte
  r1H           .byte
  .endstruct
  .endunion
  .union
  r2            .word
  .struct
  r2L           .byte
  r2H           .byte
  .endstruct
  .endunion
  .union
  r3            .word
  .struct
  r3L           .byte
  r3H           .byte
  .endstruct
  .endunion
  .union
  r4            .word
  .struct
  r4L           .byte
  r4H           .byte
  .endstruct
  .endunion
  .union
  r5            .word
  .struct
  r5L           .byte
  r5H           .byte
  .endstruct
  .endunion
  .union
  r6            .word
  .struct
  r6L           .byte
  r6H           .byte
  .endstruct
  .endunion
  .union
  r7            .word
  .struct
  r7L           .byte
  r7H           .byte
  .endstruct
  .endunion
  .union
  r8            .word
  .struct
  r8L           .byte
  r8H           .byte
  .endstruct
  .endunion
  .union
  r9            .word
  .struct
  r9L           .byte
  r9H           .byte
  .endstruct
  .endunion
  .union
  r10           .word
  .struct
  r10L          .byte
  r10H          .byte
  .endstruct
  .endunion
  .union
  r11           .word
  .struct
  r11L          .byte
  r11H          .byte
  .endstruct
  .endunion
  .union
  r12           .word
  .struct
  r12L          .byte
  r12H          .byte
  .endstruct
  .endunion
  .union
  r13           .word
  .struct
  r13L          .byte
  r13H          .byte
  .endstruct
  .endunion
  .union
  r14           .word
  .struct
  r14L          .byte
  r14H          .byte
  .endstruct
  .endunion
  .union
  r15           .word
  .struct
  r15L          .byte
  r15H          .byte
  .endstruct
  .endunion
.endstruct

; Kernal
KTEMP2          := $80          ; 2 bytes for temporary storage
IMPARM          := $82          ; Pointer for PRIMM function
FNAM            := $8A          ; Pointer to filename

; BASIC
TXTPTR          := $EE          ; Pointer into BASIC source code

; Page two

BASIC_BUF       := $0200        ; Location of command-line
BASIC_BUF_LEN   = 81            ; Maximum length of command-line

SCREEN_PTR      := $0262        ; Pointer to current row on text screen (16 bits)
STATUS          := $0287        ; Status from previous I/O operation
IN_DEV          := $028B        ; Current input device number
OUT_DEV         := $028C        ; Current output device number
FNAM_LEN        := $028F        ; Length of filename
SECADR          := $0291        ; Secondary address
DEVNUM          := $0292        ; Device number
CURS_COLOR      := $0373        ; Color under the cursor
CHARCOLOR       := $0376        ; Cursor's color nybbles (high: background, low: foreground)
RVS             := $0377        ; Reverse flag
CURS_FLAG       := $037B        ; 1 = cursor off
CURS_BLINK      := $037C        ; Blink counter
CURS_CHAR       := $037D        ; Character under the cursor
CURS_STATE      := $037E        ; Cursor blink state
CURS_X          := $0380        ; Cursor column
CURS_Y          := $0383        ; Cursor row
LLEN            := $0386        ; Line length
NLINES          := $0387        ; Number of screen lines

; BASIC
VARTAB          := $03E1        ; Pointer to start of BASIC variables
MEMSIZE         := $0259        ; Pointer to highest BASIC RAM location (+1)

; ---------------------------------------------------------------------------
; Vector and other locations

IRQVec          := $0314
BRKVec          := $0316
NMIVec          := $0318

; ---------------------------------------------------------------------------
; I/O locations

; 65C22 Versatile Interface Adapter
.struct VIA1                    ; Versatile Interface Adapter
        .org    $9F00
  PRB   .byte                   ; mouse, LED, VIC bus  (Port Register B)
  PRA   .byte                   ; keyboard, controllers  (Port Register A)
  DDRB  .byte                   ; (Data Direction Register B)
  DDRA  .byte                   ; (Data Direction Register A)
  T1    .word                   ; (Timer 1)
  T1L   .word                   ; (Timer 1 Latch)
  T2    .word                   ; (Timer 2)
  SR    .byte                   ; (Shift Register)
  ACR   .byte                   ; (Auxiliary Control Register)
  PCR   .byte                   ; (Peripheral Control Register)
  IFR   .byte                   ; (Interrupt Flags Register)
  IER   .byte                   ; (Interrupt Enable Register)
  PRA2  .byte                   ; keyboard, controllers  (PRA without handshake)
.endstruct

; 65C22 Versatile Interface Adapter
.struct VIA2
        .org    $9F10
  PRB   .byte
  PRA   .byte
  DDRB  .byte
  DDRA  .byte
  T1    .word
  T1L   .word
  T2    .word
  SR    .byte
  ACR   .byte
  PCR   .byte
  IFR   .byte
  IER   .byte
  PRA2  .byte
.endstruct

; Video Enhanced Retro Adapter
; Has audio and SPI.
.scope  VERA

  ; External registers

  .struct
                .org    $9F20
  ADDR          .faraddr        ; Address for data port access
  DATA0         .byte           ; First data port
  DATA1         .byte           ; Second data port
  CTRL          .byte           ; Control register
  IRQ_EN        .byte           ; Interrupt enable bits
  IRQ_FLAGS     .byte           ; Interrupt flags
  IRQ_RASTER    .byte           ; Line where IRQ will occur
  .endstruct
  .enum                         ; Address automatic increment amounts
  DEC0          =       (($00 << 1) | $01) << 3
  DEC1          =       (($01 << 1) | $01) << 3
  DEC2          =       (($02 << 1) | $01) << 3
  DEC4          =       (($03 << 1) | $01) << 3
  DEC8          =       (($04 << 1) | $01) << 3
  DEC16         =       (($05 << 1) | $01) << 3
  DEC32         =       (($06 << 1) | $01) << 3
  DEC64         =       (($07 << 1) | $01) << 3
  DEC128        =       (($08 << 1) | $01) << 3
  DEC256        =       (($09 << 1) | $01) << 3
  DEC512        =       (($0A << 1) | $01) << 3
  DEC40         =       (($0B << 1) | $01) << 3
  DEC80         =       (($0C << 1) | $01) << 3
  DEC160        =       (($0D << 1) | $01) << 3
  DEC320        =       (($0E << 1) | $01) << 3
  DEC640        =       (($0F << 1) | $01) << 3
  INC0          =       (($00 << 1) | $00) << 3
  INC1          =       (($01 << 1) | $00) << 3
  INC2          =       (($02 << 1) | $00) << 3
  INC4          =       (($03 << 1) | $00) << 3
  INC8          =       (($04 << 1) | $00) << 3
  INC16         =       (($05 << 1) | $00) << 3
  INC32         =       (($06 << 1) | $00) << 3
  INC64         =       (($07 << 1) | $00) << 3
  INC128        =       (($08 << 1) | $00) << 3
  INC256        =       (($09 << 1) | $00) << 3
  INC512        =       (($0A << 1) | $00) << 3
  INC40         =       (($0B << 1) | $00) << 3
  INC80         =       (($0C << 1) | $00) << 3
  INC160        =       (($0D << 1) | $00) << 3
  INC320        =       (($0E << 1) | $00) << 3
  INC640        =       (($0F << 1) | $00) << 3
  .endenum
  .enum                         ; Interrupt request flags
  VERT_SYNC     =       %00000001
  RASTER_IRQ    =       %00000010
  SPR_COLLIDED  =       %00000100
  AUDIO_LOW     =       %00001000
  .endenum
  .scope        DISP            ; Display controller
    SELECT1     =       %00000010
    .union
                .org    $9F29
    .struct
    ; These four registers are visible when the DCSEL flag = %0
    VIDEO       .byte
    HSCALE      .byte
    VSCALE      .byte
    FRAME       .byte
    .endstruct
    .struct
    ; These four registers are visible when the DCSEL flag = %1
    HSTART      .byte
    HSTOP       .byte
    VSTART      .byte
    VSTOP       .byte
    .endstruct
    .endunion
    .enum       MODE            ; Output mode
      DISABLE   =       $00
      VGA
      NTSC
      RGB                       ; Interlaced, composite sync
    .endenum
    .enum       DISABLE
    COLOR       =       %00000100       ; NTSC monochrome
    .endenum
    .enum       ENABLE
      LAYER0    =       %00010000
      LAYER1    =       %00100000
      SPRITES   =       %01000000
    .endenum
  .endscope
  .struct       L0              ; Display layer 0
                .org    $9F2D
    CONFIG      .byte
    MAP_BASE    .byte
    TILE_BASE   .byte
    HSCROLL     .word
    VSCROLL     .word
  .endstruct
  .struct       L1              ; Display layer 1
                .org    $9F34
    CONFIG      .byte
    MAP_BASE    .byte
    TILE_BASE   .byte
    HSCROLL     .word
    VSCROLL     .word
  .endstruct
  .enum                         ; Layer display modes
  TILE1BPP      =       %00000000 | $00
  TILE2BPP
  TILE4BPP
  TILE8BPP
  T256C         =       %00001000
  BITMAP1BPP    =       %00000100 | $00
  BITMAP2BPP
  BITMAP4BPP
  BITMAP8BPP
  .endenum
  .enum         MAP             ; Map geometry
    WIDTH32     =       $00 << 4
    WIDTH64     =       $01 << 4
    WIDTH128    =       $02 << 4
    WIDTH256    =       $03 << 4
    HEIGHT32    =       $00 << 6
    HEIGHT64    =       $01 << 6
    HEIGHT128   =       $02 << 6
    HEIGHT256   =       $03 << 6
  .endenum
  .enum         TILE            ; Tile geometry
    WIDTH8      =       $00
    WIDTH16     =       $01
    WIDTH320    =       WIDTH8
    WIDTH640    =       WIDTH16
    HEIGHT8     =       $00 << 1
    HEIGHT16    =       $01 << 1
  .endenum
  .scope        PCM             ; Pulse-Code Modulator
    .struct
                .org    $9F3B
    CTRL        .byte
    RATE        .byte
    DATA        .byte
    .endstruct
    .enum
    STEREO      =       %00010000
    BITS16      =       %00100000
    RESET       =       %10000000
    .endenum
  .endscope
  .scope        SPI
    .struct
                .org    $9F3E
    DATA        .byte
    CTRL        .byte
    .endstruct
    .enum
    SELECT      =       %00000001
    SLOW        =       %00000010
    .endenum
  .endscope

  ; Internal RAM and registers

  .struct
                .org    $000000
  VRAM          .res    $020000 ; 128 Kibibytes
  .endstruct
  .scope        PSG             ; Programmable Sound Generator
    .struct
    PITCH       .word
    VOL         .byte           ; Right, left sides; volume
    WAVEFORM    .byte           ; Wave shape, pulse width
    .endstruct
    LEFT        =       %01 << 6
    RIGHT       =       %10 << 6
    .enum
    PULSE       =       $00 << 6
    SAWTOOTH    =       $01 << 6
    TRIANGLE    =       $02 << 6
    NOISE       =       $03 << 6
    .endenum
    .struct
                .org    $01F9C0
    VOICES      .res    $10 * 4
    .endstruct
  .endscope
  .struct
                .org    $01FA00
  PALETTE       .word   $0100
  .endstruct
  .scope        SPRITE
    .struct                     ; Sprite attributes
    ADDR        .addr           ; Address and color mode
    XX          .word           ; Co-ordinates
    YY          .word
    Z_FLIP      .byte           ; Collision mask, Z-depth, flip bits
    SIZE_PAL    .byte
    .endstruct
    .enum       FLIP
      NONE      =       %00000000
      HORIZ
      VERT
      BOTH
    .endenum
    .enum       DEPTH
      DISABLE   =       $00 << 2
      CANVAS    =       $01 << 2
      LAYER0    =       $02 << 2
      LAYER1    =       $03 << 2
    .endenum
    .enum                       ; Sprite geometry
    WIDTH8      =       $00 << 4
    WIDTH16     =       $01 << 4
    WIDTH32     =       $02 << 4
    WIDTH64     =       $03 << 4
    HEIGHT8     =       $00 << 6
    HEIGHT16    =       $01 << 6
    HEIGHT32    =       $02 << 6
    HEIGHT64    =       $03 << 6
    COLORS16    =       $00 << 7
    COLORS256   =       $01 << 7
    .endenum
  .endscope
  .struct
                .org    $01FC00
  SPRITES       .res    128 * 8
  .endstruct
.endscope

; YM2151 audio chip
.struct YM2151
                .org    $9F40
  ADDR          .byte
  .union
  DATA          .byte
  STATUS        .byte
  .endunion
.endstruct

; X16 Emulator device
; This device doesn't exist on the real machine.
.struct EMULATOR
                .org    $9FB0
  DEBUG         .byte           ; Boolean: debugging enabled
  VERALOG       .byte           ; Boolean: log VERA activity
  KEYBOARDLOG   .byte           ; Boolean: log keyboard data
  ECHO          .byte           ; Type of echo that's enabled
  SAVEXIT       .byte           ; Boolean: save machine state on exit
  GIFREC        .byte           ; Method of recording GIF movie
                .res    2
  CYCLECOUNT    .dword          ; Running count of CPU cycles (Read-Only)
                .res    1
  KEYMAP        .byte           ; Current keyboard layout number (Read-Only)
  DETECT        .byte   2       ; If is "16" string, then running on emulator (RO)
.endstruct

; ---------------------------------------------------------------------------
; Banked RAM and ROM

TIMER           := $A03B        ; (bank 0) 60 Hz. timer (3 bytes, big-endian)

.struct BANK
        .org    $A000
  RAM   .res    $2000           ; 8 Kibibyte window into 512 Kibibytes or 2048 Kibibytes
  ROM   .res    $4000           ; 16 Kibibyte window into 128 Kibibytes
.endstruct
