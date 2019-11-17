;
; CX16 definitions
;

; ---------------------------------------------------------------------------
; Constants

.enum COLOR
  BLACK         = $00
  WHITE
  RED
  CYAN
  VIOLET
  PURPLE        = VIOLET
  GREEN
  BLUE
  YELLOW
  ORANGE
  BROWN
  LIGHTRED
  GRAY1
  GRAY2
  LIGHTGREEN
  LIGHTBLUE
  GRAY3
.endenum

; Special keys
.enum KEY
  F1    = $85
  F3
  F5
  F7
  F2
  F4
  F6
  F8
  F9    = $10
  F10   = $15
  F11
  F12
.endenum

; ---------------------------------------------------------------------------
; Zero page

; Kernal
FNAM            := $84          ; Pointer to filename
KTEMP2          := $86          ; 2 bytes for temporary storage
SCREEN_PTR      := $88          ; Pointer to current row on text screen (16 bits)
IMPARM          := $8A          ; Pointer for PRIMM function

; BASIC
TXTPTR          := $EE          ; Pointer into BASIC source code

; Page two

BASIC_BUF       := $0200        ; Location of command-line
BASIC_BUF_LEN   = 81            ; Maximum length of command-line

CURS_COLOR      := $027E        ; Color under the cursor
CHARCOLOR       := $0286        ; Cursor's color nybbles (high: background, low: foreground)
STATUS          := $0287        ; Status from previous I/O operation
IN_DEV          := $028E        ; Current input device number
OUT_DEV         := $028F        ; Current output device number
TIME            := $0292        ; 60 Hz. clock (3 bytes, big-endian)
FNAM_LEN        := $0298        ; Length of filename
SECADR          := $029A        ; Secondary address
DEVNUM          := $029B        ; Device number
KEY_COUNT       := $029E        ; Number of keys in input buffer
RVS             := $029F        ; Reverse flag
CURS_FLAG       := $02A3        ; 1 = cursor off
CURS_BLINK      := $02A4        ; Blink counter
CURS_CHAR       := $02A5        ; Character under the cursor
CURS_STATE      := $02A6        ; Cursor blink state
CURS_X          := $02A8        ; Cursor column
CURS_Y          := $02AB        ; Cursor row
LLEN            := $02AE        ; Line length
NLINES          := $02AF        ; Number of screen lines
JOY1            := $02BC        ; 3 bytes of NES/SNES gamepad data
JOY2            := $02BF

; BASIC
VARTAB          := $02DD        ; Pointer to start of BASIC variables
MEMSIZE         := $02E5        ; Pointer to highest BASIC RAM location (+1)

; Kernal mouse
MSEPAR          := $0371        ; mouse: $8x=sprite on, 1/2: scale
MOUSEL          := $0372        ; min. x co-ordinate
MOUSER          := $0374        ; max. x co-ordinate
MOUSET          := $0376        ; min. y co-ordinate
MOUSEB          := $0378        ; max. y co-ordinate
MOUSEX          := $037A        ; x co-ordinate
MOUSEY          := $037C        ; y co-ordinate
MOUSEBT         := $037E        ; buttons (bits 2: middle, 1: right, 0: left)

; ---------------------------------------------------------------------------
; Vector and other locations

IRQVec          := $0314
BRKVec          := $0316
NMIVec          := $0318

; ---------------------------------------------------------------------------
; I/O locations

; Video Enhanced Retro Adapter
; Has audio, SPI, and UART.
.scope VERA
  ; External registers
  .struct
                .org    $9F20
  ADDR          .faraddr        ; Address for data port access
  DATA0         .byte           ; First data port
  DATA1         .byte           ; Second data port
  CTRL          .byte           ; Control register
  IRQ_EN        .byte           ; Interrupt enable bits
  IRQ_FLAGS     .byte           ; Interrupt flags
  .endstruct
  .enum                         ; Address automatic increment amounts
  INC0          =       0 << 4
  INC1          =       1 << 4
  INC2          =       2 << 4
  INC4          =       3 << 4
  INC8          =       4 << 4
  INC16         =       5 << 4
  INC32         =       6 << 4
  INC64         =       7 << 4
  INC128        =       8 << 4
  INC256        =       9 << 4
  INC512        =       10 << 4
  INC1024       =       11 << 4
  INC2048       =       12 << 4
  INC4096       =       13 << 4
  INC8192       =       14 << 4
  INC16384      =       15 << 4
  .endenum
  .enum                         ; Interrupt request flags
  VERT_SYNC     =       %00000001
  RASTER        =       %00000010
  SPR_COLLIDED  =       %00000100
  UART_IRQ      =       %00001000
  .endenum
  ; Internal RAM and registers
  VRAM          :=      $000000
  .scope        COMPOSER        ; Display composer
    .struct
                .org    $0F0000
    VIDEO       .byte
    HSCALE      .byte
    VSCALE      .byte
    FRAME       .byte
    HSTART_LO   .byte
    HSTOP_LO    .byte
    VSTART_LO   .byte
    VSTOP_LO    .byte
    STRTSTOP_HI .byte
    IRQ_LINE    .word
    .endstruct
    .enum       MODE            ; Output mode
      DISABLE   =       0
      VGA
      NTSC
      RGB                       ; Interlaced, composite sync
    .endenum
    .enum
    ENABLE_COLOR        =       0 << 2
    DISABLE_COLOR       =       1 << 2  ; NTSC monochrome
    .endenum
  .endscope
  PALETTE       :=      $0F1000
  .struct       L0              ; Layer 0 registers
                .org    $0F2000
    CTRL0       .byte           ; Display mode control
    CTRL1       .byte           ; Geometry control
    MAP_BASE    .addr
    TILE_BASE   .addr
    HSCROLL     .word           ; Horizontal scroll
    VSCROLL     .word           ; Vertical scroll
  .endstruct
  .struct       L1              ; Layer 1 registers (same as layer 0)
                .org    $0F3000
    CTRL0       .byte
    CTRL1       .byte
    MAP_BASE    .addr
    TILE_BASE   .addr
    HSCROLL     .word
    VSCROLL     .word
  .endstruct
  .enum         MAP             ; Map geometry
    WIDTH32     =       0
    WIDTH64
    WIDTH128
    WIDTH256
    HEIGHT32    =       0 << 2
    HEIGHT64    =       1 << 2
    HEIGHT128   =       2 << 2
    HEIGHT256   =       3 << 2
  .endenum
  .scope        TILE            ; Tile geometry
    .enum
    WIDTH8      =       0 << 4
    WIDTH16     =       1 << 4
    WIDTH320    =       WIDTH8
    WIDTH640    =       WIDTH16
    HEIGHT8     =       0 << 5
    HEIGHT16    =       1 << 5
    .endenum
    .enum       FLIP
      NONE      =       0 << 2
      HORIZ     =       1 << 2
      VERT      =       2 << 2
      BOTH      =       3 << 2
    .endenum
  .endscope
  .enum         DMODE           ; Display modes
    TEXT16      =       0 << 5
    TEXT256     =       1 << 5
    TILE4       =       2 << 5
    TILE16      =       3 << 5
    TILE256     =       4 << 5
    BITMAP4     =       5 << 5
    BITMAP16    =       6 << 5
    BITMAP256   =       7 << 5
  .endenum
  .scope        SPRITE
    .struct
                .org    $0F4000
    CTRL        .byte           ; Enables sprites
    COLLISION   .byte
    .endstruct
    .enum       FLIP
      NONE      =       0
      HORIZ
      VERT
      BOTH
    .endenum
    .enum                       ; Sprite geometry
    WIDTH8      =       0 << 4
    WIDTH16     =       1 << 4
    WIDTH32     =       2 << 4
    WIDTH64     =       3 << 4
    HEIGHT8     =       0 << 6
    HEIGHT16    =       1 << 6
    HEIGHT32    =       2 << 6
    HEIGHT64    =       3 << 6
    COLORS16    =       0 << 7
    COLORS256   =       1 << 7
    .endenum
    .enum       DEPTH
      DISABLE   =       0 << 2
      CANVAS    =       1 << 2
      LAYER0    =       2 << 2
      LAYER1    =       3 << 2
    .endenum
    ATTRIB      :=      $0F5000 ; Sprite attributes
  .endscope
  AUDIO         :=      $0F6000
  .scope        SPI
    .struct
                .org    $0F7000
    DATA        .byte
    CONTROL     .byte
    .endstruct
    .enum
    DESELECT    =       0
    SELECT
    BUSY_MASK   =       1 << 1
    .endenum
  .endscope
  .scope        UART            ; Universal Asyncronous Receiver Transmitter
    .struct
                .org    $0F8000
    DATA        .byte
    STATUS      .byte
    BPS_DIV     .word
    .endstruct
    .enum       MASK
      RECEIVE   =       1 << 0
      TRANSMIT  =       1 << 1
    .endenum
  .endscope
.endscope

; 65C22
.struct VIA1                    ; Versatile Interface Adapter
        .org    $9F60
  PRB   .byte                   ; ROM bank, IEC  (Port Register B)
  PRA   .byte                   ; RAM bank  (Port Register A)
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
  PRA2  .byte                   ; RAM bank  (Port Register A without handshaking)
.endstruct

; 65C22
.struct VIA2
        .org    $9F70
  PRB   .byte                   ; Mouse communication ?
  PRA   .byte                   ; NES controller communication
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

; Real-Time Clock

; X16 Emulator device
; This device doesn't exist on the real machine.
.struct EMULATOR
                .org    $9FB0
  DEBUG         .byte           ; Boolean: debugging enabled
  VERALOG       .byte           ; Boolean: log VERA activity
  KEYBOARDLOG   .byte           ; Boolean: log keyboard data
  ECHO          .byte           ; Type of echo that's enabled
  SAVEXIT       .byte           ; Boolean: save on exit
  GIFREC        .byte           ; Method of recording GIF movie
                .org    $9FBD
  KEYMAP        .byte           ; Current keyboard layout number (Read-Only)
  DETECT        .byte   2       ; If is "16" string, then running on emulator (RO)
.endstruct
