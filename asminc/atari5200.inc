;-------------------------------------------------------------------------
; Atari 5200 System Equates
; by Christian Groessler <chris@groessler.org>
; taken from EQUATES.INC from Atari Inc.
;-------------------------------------------------------------------------

;-------------------------------------------------------------------------
; ATASCII CHARACTER DEFS
;-------------------------------------------------------------------------

ATEOL   = $9B      ; END-OF-LINE, used by CONIO

;-------------------------------------------------------------------------
; CONIO CHARACTER DEFS
;-------------------------------------------------------------------------

CH_ULCORNER = $0B  ; '+' sign
CH_URCORNER = $0B
CH_LLCORNER = $0B
CH_LRCORNER = $0B
CH_HLINE    = $0D  ; dash
CH_VLINE    = $01  ; exclamation mark

;-------------------------------------------------------------------------
; Zero Page
;-------------------------------------------------------------------------

POKMSK  =  $00     ; Mask for Pokey IRQ enable
RTCLOK  =  $01     ; 60 hz. clock
JUMP    =  $01
CRITIC  =  $03     ; Critical section
ATRACT  =  $04     ; Attract Mode

SDLSTL  =  $05     ; DLISTL Shadow
SDLSTH  =  $06     ; DLISTH  "
SDMCTL  =  $07     ; DMACTL  "

PCOLR0  =  $08     ; COLPM0 Shadow
PCOLR1  =  $09     ; COLPM1  "
PCOLR2  =  $0A     ; COLPM2  "
PCOLR3  =  $0B     ; COLPM3  "

COLOR0  =  $0C     ; COLPF0 Shadow
COLOR1  =  $0D     ; COLPF1  "
COLOR2  =  $0E     ; COLPF2  "
COLOR3  =  $0F     ; COLPF3  "
COLOR4  =  $10     ; COLBK   "

PADDL0  =  $11     ; POT0 Shadow
PADDL1  =  $12     ; POT1  "
PADDL2  =  $13     ; POT2  "
PADDL3  =  $14     ; POT3  "
PADDL4  =  $15     ; POT4  "
PADDL5  =  $16     ; POT5  "
PADDL6  =  $17     ; POT6  "
PADDL7  =  $18     ; POT7  "

; cc65 runtime zero page variables

ROWCRS_5200     =       $19
COLCRS_5200     =       $1A
SAVMSC          =       $1B             ; pointer to screen memory (conio)

;-------------------------------------------------------------------------
; Page #2
;-------------------------------------------------------------------------

;Interrupt Vectors

VIMIRQ  =  $0200   ; Immediate IRQ
                      ; Preset $FC03 (SYSIRQ)
VVBLKI  =  $0202   ; Vblank immediate
                      ; Preset $FCB8 (SYSVBL)
VVBLKD  =  $0204   ; Vblank deferred
                      ; Preset $FCB2 (XITVBL)
VDSLST  =  $0206   ; Display List
                      ; Preset $FEA1 (OSDLI)
VKYBDI  =  $0208   ; Keyboard immediate
                      ; Preset $FD02 (SYSKBD)
VKYBDF  =  $020A   ; Deferred Keyboard
                      ; Preset $FCB2 (XITVBL)
VTRIGR  =  $020C   ; Soft Trigger
VBRKOP  =  $020E   ; BRK Opcode
VSERIN  =  $0210   ; Serial in Ready
VSEROR  =  $0212   ; Serial Out Ready
VSEROC  =  $0214   ; Serial Output complete
VTIMR1  =  $0216   ; Pokey Timer 1
VTIMR2  =  $0218   ; Pokey Timer 2
VTIMR4  =  $021A   ; Pokey Timer 4



;-------------------------------------------------------------------------
; CTIA/GTIA Address Equates
;-------------------------------------------------------------------------

GTIA    = $C000    ; CTIA/GTIA area
.include "atari_gtia.inc"

;-------------------------------------------------------------------------
; ANTIC Address Equates
;-------------------------------------------------------------------------

ANTIC     = $D400  ; ANTIC area
.include  "atari_antic.inc"

;-------------------------------------------------------------------------
; POKEY Address Equates
;-------------------------------------------------------------------------

POKEY     = $E800  ; POKEY area
.include  "atari_pokey.inc"

;-------------------------------------------------------------------------
; conio color defines
;-------------------------------------------------------------------------

COLOR_WHITE = 0
COLOR_RED   = 1
COLOR_GREEN = 2
COLOR_BLACK = 3

;-------------------------------------------------------------------------
; Cartridge Parameters
;-------------------------------------------------------------------------

CARTNM  =  $BFE8   ; Cartridge Name Area
COPYD   =  $BFFC   ; Copyright Decade in Cart
COPYR   =  $BFFD   ; Copyright Year in Cart
                      ; $FF=Diagnostic Cart
GOCART  =  $BFFE   ; Cartridge Start Vector

CHRORG  =  $F800   ; Character Generator Base
