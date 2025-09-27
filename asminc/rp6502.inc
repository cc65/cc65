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
RIA_OP_PHI2               := $02
RIA_OP_CODEPAGE           := $03
RIA_OP_LRAND              := $04
RIA_OP_STDIN_OPT          := $05
RIA_OP_ERRNO_OPT          := $06
RIA_OP_CLOCK              := $0F
RIA_OP_CLOCK_GETRES       := $10
RIA_OP_CLOCK_GETTIME      := $11
RIA_OP_CLOCK_SETTIME      := $12
RIA_OP_CLOCK_GETTIMEZONE  := $13
RIA_OP_OPEN               := $14
RIA_OP_CLOSE              := $15
RIA_OP_READ_XSTACK        := $16
RIA_OP_READ_XRAM          := $17
RIA_OP_WRITE_XSTACK       := $18
RIA_OP_WRITE_XRAM         := $19
RIA_OP_LSEEK              := $1A
RIA_OP_UNLINK             := $1B
RIA_OP_RENAME             := $1C

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
