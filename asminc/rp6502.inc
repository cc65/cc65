; Picocomputer 6502 general defines

; RIA UART
RIA_READY       := $FFE0 ; TX=$80 RX=$40
RIA_TX          := $FFE1
RIA_RX          := $FFE2

; VSYNC from PIX VGA
RIA_VSYNC       := $FFE3

; RIA XRAM portal 0
RIA_RW0         := $FFE4
RIA_STEP0       := $FFE5
RIA_ADDR0       := $FFE6

; RIA XRAM portal 1
RIA_RW1         := $FFE8
RIA_STEP1       := $FFE9
RIA_ADDR1       := $FFEA

; RIA OS fastcall
RIA_XSTACK      := $FFEC
RIA_ERRNO       := $FFED
RIA_OP          := $FFEF
RIA_IRQ         := $FFF0
RIA_SPIN        := $FFF1
RIA_BUSY        := $FFF2 ; Bit $80
RIA_A           := $FFF4
RIA_X           := $FFF6
RIA_SREG        := $FFF8

; RIA OS operation numbers
RIA_OP_EXIT               := $FF
RIA_OP_ZXSTACK            := $00
RIA_OP_XREG               := $01
RIA_OP_ARGV               := $08
RIA_OP_EXEC               := $09
RIA_OP_ATTR_GET           := $0A
RIA_OP_ATTR_SET           := $0B
RIA_OP_OPEN               := $14
RIA_OP_CLOSE              := $15
RIA_OP_READ_XSTACK        := $16
RIA_OP_READ_XRAM          := $17
RIA_OP_WRITE_XSTACK       := $18
RIA_OP_WRITE_XRAM         := $19
RIA_OP_LSEEK              := $1A
RIA_OP_LSEEK_CC65         := $1A
RIA_OP_UNLINK             := $1B
RIA_OP_RENAME             := $1C
RIA_OP_LSEEK_LLVM         := $1D
RIA_OP_SYNCFS             := $1E
RIA_OP_STAT               := $1F
RIA_OP_OPENDIR            := $20
RIA_OP_READDIR            := $21
RIA_OP_CLOSEDIR           := $22
RIA_OP_TELLDIR            := $23
RIA_OP_SEEKDIR            := $24
RIA_OP_REWINDDIR          := $25
RIA_OP_CHMOD              := $26
RIA_OP_UTIME              := $27
RIA_OP_MKDIR              := $28
RIA_OP_CHDIR              := $29
RIA_OP_CHDRIVE            := $2A
RIA_OP_GETCWD             := $2B
RIA_OP_SETLABEL           := $2C
RIA_OP_GETLABEL           := $2D
RIA_OP_GETFREE            := $2E
RIA_OP_RLN_LASTKEY        := $30
RIA_OP_RLN_PEEK           := $31
RIA_OP_RLN_POKE           := $32
RIA_OP_GMTIME             := $3A
RIA_OP_LOCALTIME          := $3B
RIA_OP_MKTIME             := $3C
RIA_OP_STRFTIME           := $3D
RIA_OP_TIME_SET           := $3E
RIA_OP_TIME_GET           := $3F

; RIA attribute IDs
RIA_ATTR_ERRNO_OPT        := $00
RIA_ATTR_PHI2_KHZ         := $01
RIA_ATTR_CODE_PAGE        := $02
RIA_ATTR_RLN_LENGTH       := $03
RIA_ATTR_LRAND            := $04
RIA_ATTR_BEL              := $05
RIA_ATTR_LAUNCHER         := $06
RIA_ATTR_EXIT_CODE        := $07
RIA_ATTR_SIGINT           := $08
RIA_ATTR_RLN_CAPS         := $09
RIA_ATTR_RLN_WIDTH        := $0A
RIA_ATTR_RLN_HEIGHT       := $0B
RIA_ATTR_RLN_SUPPRESS_NL  := $0C
RIA_ATTR_CLK_RUN_MS       := $10
RIA_ATTR_CLK_RUN_CS       := $11
RIA_ATTR_CLK_RUN_DS       := $12
RIA_ATTR_CLK_RUN_S        := $13

; 6522 VIA
VIA             := $FFD0        ; VIA base address
VIA_PB          := VIA+$0       ; Port register B
VIA_PA1         := VIA+$1       ; Port register A
VIA_PRB         := VIA+$0       ; *** Deprecated ***
VIA_PRA         := VIA+$1       ; *** Deprecated ***
VIA_DDRB        := VIA+$2       ; Data direction register B
VIA_DDRA        := VIA+$3       ; Data direction register A
VIA_T1CL        := VIA+$4       ; Timer 1, low byte
VIA_T1CH        := VIA+$5       ; Timer 1, high byte
VIA_T1LL        := VIA+$6       ; Timer 1 latch, low byte
VIA_T1LH        := VIA+$7       ; Timer 1 latch, high byte
VIA_T2CL        := VIA+$8       ; Timer 2, low byte
VIA_T2CH        := VIA+$9       ; Timer 2, high byte
VIA_SR          := VIA+$A       ; Shift register
VIA_CR          := VIA+$B       ; Auxiliary control register
VIA_PCR         := VIA+$C       ; Peripheral control register
VIA_IFR         := VIA+$D       ; Interrupt flag register
VIA_IER         := VIA+$E       ; Interrupt enable register
VIA_PA2         := VIA+$F       ; Port register A w/o handshake
