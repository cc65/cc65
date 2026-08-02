
;-----------------------------------------------------------------------------
; Zero page

INTEXTPG     := $1601    ; Interpreter extended address offset


;-----------------------------------------------------------------------------
; Vectors



;-----------------------------------------------------------------------------
; Hardware

KBD         :=  $C000   ; last key pressed (KA Data)
KBDBFLG     :=  $C008   ; KB data
KBDSTRB     :=  $C010   ; RW keyboard strobe
SPKR        :=  $C030   ; RW toggle speaker (Apple II type)
SPKRIII     :=  $C040   ; RW beeps speaker (Apple /// type)

;-----------------------------------------------------------------------------
; Video
; 
; Mode                             HIRES  MIX    TEXTGR
; 40 Char                (B/W)     0      0      0
; 40 Char                (Color    0      0      1
; 80 Char                (B/W)     0      1      0
; 80 Char                (B/W)     0      1      1
; AII Hires 280x192      (B/W)     1      0      0
; FGD/BKGD HIRES 280x192 (Color)   1      0      1
; Super HIRES 560x192    (B/W)     1      1      0
; AHIRES 140x192         (Color)   1      1      1

TEXTGRCLR   :=  $C050   ; Clear TEXT/GR mode
TEXTGRSET   :=  $C051   ; Set TEXT/GR mode
MIXCLR      :=  $C052   ; Clear MIX mode
MIXSET      :=  $C053   ; Set MIX mode
PAGE2CLR    :=  $C054   ; Clear PAGE2 mode
PAGE2SET    :=  $C055   ; Set PAGE2 mode
HIRESCLR    :=  $C056   ; Clear HIRES mode
HIRESSET    :=  $C057   ; Set HIRES mode


PDL0CLR     :=  $C058   ; Clear PDL0 (A/D Addr 0)
PDL0SET     :=  $C059   ; Set PDL0 (A/D Addr 0)
PDL2CLR     :=  $C05A   ; Clear PDL2 (A/D Addr 2)
PDL2SET     :=  $C05B   ; Set PDL2 (A/D Addr 2)
PDLENCLR    :=  $C05C   ; Clear PDLEN (A/D Ramp Start)
SETPDLEN    :=  $C05D   ; Set PDLEN (A/D Ramp Start)
PDL1CLR     :=  $C05E   ; Clear PDL1 (A/D Addr 1)
PDL1SET     :=  $C05F   ; Set PDL1 (A/D Addr 1)
READSW0     :=  $C060   ; Read SW0
READSW1     :=  $C061   ; Read SW1/MGNSW
READSW2     :=  $C062   ; Read SW2
READSW3     :=  $C063   ; Read SW3/SCO
JOYRDY      :=  $C066   ; Read PDLOT (A/D Ramp Stop)
CLOCK       :=  $C070   ; clock
PHASOFF     :=  $C080   ; 
PHASON      :=  $C081   ; 
MOTOROFF    :=  $C088   ; 
MOTORON     :=  $C089   ; 
DRV0EN      :=  $C08A   ; 
DRV1EN      :=  $C08B   ; 
Q6L         :=  $C08C   ; 
Q6H         :=  $C08D   ; 
Q7L         :=  $C08E   ; 
DSA0CLR     :=  $C0D0   ; Clear Drive Select A0
DSA0SET     :=  $C0D1   ; Set Drive Select A0
DSA1CLR     :=  $C0D2   ; Clear Drive Select A1
DSA1SET     :=  $C0D3   ; Set Drive Select A1
EN1INTCLR   :=  $C0D4   ; 
EN1INTSET   :=  $C0D5   ; 
SIDE2CLR    :=  $C0D6   ; 
SIDE2SET    :=  $C0D7   ; 
SCRCLR      :=  $C0D8   ; Clear Smooth Scroll  (to turn smooth scroll off)
SCRSET      :=  $C0D9   ; Set Smooth Scroll  (to turn smooth scroll on)
ENCWRTCLR   :=  $C0DA   ; Clear Char Set writing
ENCWRTSET   :=  $C0DB   ; Set Char Set writing
ENSELCLR    :=  $C0DC   ; Clear enable silentype port (ENSEL)
ENSELSET    :=  $C0DD   ; Set enable silentype port (ENSEL)
ENSIOCLR    :=  $C0DE   ; Clear enable silentype port (ENSIO)
ENSIOSET    :=  $C0DF   ; Set enable silentype port (ENSIO)
ACIADATA    :=  $C0F0   ; ACIA DATA REGISTER
ACIASTAT    :=  $C0F1   ; ACIA STATUS REGISTER
ACIACMD     :=  $C0F2   ; ACIA COMMAND REGISTER
ACIACTL     :=  $C0F3   ; ACIA CONTROL REGISTER

; Other hardware registers
Z_REG       :=  $FFD0   ; zero page register
D_DDRB      :=  $FFD2   ; data direction register B
D_DDRA      :=  $FFD3   ; data direction register A
D_TIMER1C_L :=  $FFD4
D_TIMER1C_H :=  $FFD5
D_TIMER1L_L :=  $FFD6
D_TIMER1L_H :=  $FFD7
D_TIMER2C_L :=  $FFD8
D_TIMER2C_H :=  $FFD9
D_ACR       :=  $FFDB
D_PCR       :=  $FFDC
D_IFR       :=  $FFDD
D_IER       :=  $FFDE
E_REG       :=  $FFDF   ; environment register
E_IORB      :=  $FFE0
E_DDRB      :=  $FFE2
E_DDRA      :=  $FFE3
E_TIMER1C_L :=  $FFE6
E_TIMER1C_H :=  $FFE7
E_TIMER1L_L :=  $FFE6
E_TIMER1L_H :=  $FFE7
E_TIMER2C_L :=  $FFE8
E_TIMER2C_H :=  $FFE9
E_ACR       :=  $FFEB
E_PCR       :=  $FFEC
E_IFR       :=  $FFED
E_IER       :=  $FFEE
B_REG       :=  $FFEF   ; bank switch register


;-----------------------------------------------------------------------------
; CONSOLE Driver commands
CONSOLE_CURSOR_ON    := 5
CONSOLE_CURSOR_OFF   := 6
CONSOLE_CURSOR_LEFT  := 8
CONSOLE_CURSOR_RIGHT := 9
CONSOLE_CURSOR_DOWN  := 10
CONSOLE_CURSOR_UP    := 11
CONSOLE_TEXT_MODE    := 16
CONSOLE_NORMAL       := 17
CONSOLE_INVERSE      := 18
CONSOLE_FORE_COLOR   := 19
CONSOLE_BACK_COLOR   := 20
CONSOLE_SCREEN_SYNC  := 22
CONSOLE_CLR_VIEWPORT := 28
CTRL_SCREEN_ECHO     := 11
STAT_KEYSTROKE_COUNT := 5
STAT_ANY_KEY         := 8
STAT_CURSOR_POS      := 16
STAT_READ_SCR        := 17

VIDEOMODE_80x24      := 2
