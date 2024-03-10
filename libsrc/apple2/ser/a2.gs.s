;
; Serial driver for the Apple IIgs Zilog Z8530.
;
; Colin Leroy-Mira <colin@colino.net>, 2023
;
; This software is licensed under the same license as cc65,
; the zlib license (see LICENSE file).
;
; Documentation from http://www.applelogic.org/files/Z8530UM.pdf (pages
; referred to where applicable)
; and https://gswv.apple2.org.za/a2zine/Utils/Z8530_SCCsamples_info.txt



        .setcpu         "65816"

        .include        "zeropage.inc"
        .include        "ser-kernel.inc"
        .include        "ser-error.inc"

        .macpack        module

; ------------------------------------------------------------------------
; Header. Includes jump table

        .ifdef  __APPLE2ENH__
        module_header   _a2e_gs_ser
        .else
        module_header   _a2_gs_ser
        .endif

        ; Driver signature
        .byte   $73, $65, $72           ; "ser"
        .byte   SER_API_VERSION         ; Serial API version number

        ; Library reference
        .addr   $0000

        ; Jump table
        .addr   SER_INSTALL
        .addr   SER_UNINSTALL
        .addr   SER_OPEN
        .addr   SER_CLOSE
        .addr   SER_GET
        .addr   SER_PUT
        .addr   SER_STATUS
        .addr   SER_IOCTL
        .addr   SER_IRQ

;----------------------------------------------------------------------------
; Global variables

        .bss

RecvHead:       .res    1               ; Head of receive buffer
RecvTail:       .res    1               ; Tail of receive buffer
RecvFreeCnt:    .res    1               ; Number of bytes in receive buffer
SendHead:       .res    1               ; Head of send buffer
SendTail:       .res    1               ; Tail of send buffer
SendFreeCnt:    .res    1               ; Number of bytes in send buffer

Stopped:        .res    1               ; Flow-stopped flag
RtsOff:         .res    1
HSType:         .res    1               ; Flow-control type

RecvBuf:        .res    256             ; Receive buffers: 256 bytes
SendBuf:        .res    256             ; Send buffers: 256 bytes

CurClockSource: .res    1               ; Whether to use BRG or RTxC for clock

        .data

Opened:         .byte   $00             ; 1 when opened
Channel:        .byte   $00             ; Channel B by default
CurChanIrqFlags:.byte   $00

SerFlagOrig:    .byte   $00

RxBitTable:     .byte   %00000000       ; SER_BITS_5, in WR_RX_CTRL (WR3)
                .byte   %10000000       ; SER_BITS_6  (Ref page 5-7)
                .byte   %01000000       ; SER_BITS_7
                .byte   %11000000       ; SER_BITS_8

TxBitTable:     .byte   %00000000       ; SER_BITS_5, in WR_TX_CTRL (WR5)
                .byte   %01000000       ; SER_BITS_6  (Ref page 5-9)
                .byte   %00100000       ; SER_BITS_7
                .byte   %01100000       ; SER_BITS_8

        .rodata

ClockMultiplier:.byte   %01000000       ; Clock x16 (300-57600bps, WR4, ref page 5-8)
                .byte   %10000000       ; Clock x32 (115200bps, ref page 5-8)

ClockSource:    .byte   %01010000       ; Use baud rate generator (ch. B) (WR11, page 5-17)
                .byte   %00000000       ; Use RTxC (115200bps) (ch. B)
                .byte   %11010000       ; Use baud rate generator (ch. A)
                .byte   %10000000       ; Use RTxC (115200bps) (ch. A)

BrgEnabled:     .byte   %00000001       ; Baud rate generator on (WR14, page 5-19)
                .byte   %00000000       ; BRG Off

ChanIrqFlags:   .byte %00000101         ; ANDed (RX/special IRQ, ch. B) (page 5-25)
                .byte %00101000         ; ANDed (RX/special IRQ, ch. A)

ChanIrqMask:    .byte %00000111         ; Ch. B IRQ flags mask
                .byte %00111000         ; Ch. A IRQ flags mask

BaudTable:                              ; bit7 = 1 means setting is invalid
                                        ; Indexes cc65 RS232 SER_BAUD enum
                                        ; into WR12/13 register values
                                        ; (Ref page 5-18 and 5-19)
                .word   $FFFF           ; SER_BAUD_45_5
                .word   $FFFF           ; SER_BAUD_50
                .word   $FFFF           ; SER_BAUD_75
                .word   $FFFF           ; SER_BAUD_110
                .word   $FFFF           ; SER_BAUD_134_5
                .word   $FFFF           ; SER_BAUD_150
                .word   $017E           ; SER_BAUD_300
                .word   $FFFF           ; SER_BAUD_600
                .word   $005E           ; SER_BAUD_1200
                .word   $FFFF           ; SER_BAUD_1800
                .word   $002E           ; SER_BAUD_2400
                .word   $FFFF           ; SER_BAUD_3600
                .word   $0016           ; SER_BAUD_4800
                .word   $FFFF           ; SER_BAUD_7200
                .word   $000A           ; SER_BAUD_9600
                .word   $0004           ; SER_BAUD_19200
                .word   $0001           ; SER_BAUD_38400
                .word   $0000           ; SER_BAUD_57600
                .word   $0000           ; SER_BAUD_115200 (constant unused at that speed)
                .word   $FFFF           ; SER_BAUD_230400

; About the speed selection: either we use the baud rate generator:
; - Load the time constants from BaudTable into WR12/WR13
; - Setup the TX/RX clock source to BRG (ClockSource into WR11)
; - Setup the clock multiplier (WR4)
; - Enable the baud rate generator (WR14)
; In this case, the baud rate will be:
;    rate = crystal_clock/(2+BRG_time_constant))/(2*clock_multiplier)
; Example: (3686400/(2+0x0004)) / (2*16) = 19200 bps
;
; Or we don't use the baud rate generator:
; - Setup the TX/RX clock source to RTxC
; - Setup the clock multiplier
; - Disable the baud rate generator
; - WR12 and 13 are ignored
; In this case, the baud rate will be:
;    rate = crystal_clock/clock_multiplier
; Example: 3686400/32 = 115200 bps

StopTable:      .byte   %00000100       ; SER_STOP_1, in WR_TX_RX_CTRL (WR4)
                .byte   %00001100       ; SER_STOP_2  (Ref page 5-8)

ParityTable:    .byte   %00000000       ; SER_PAR_NONE, in WR_TX_RX_CTRL (WR4)
                .byte   %00000001       ; SER_PAR_ODD   (Ref page 5-8)
                .byte   %00000011       ; SER_PAR_EVEN
                .byte   $FF             ; SER_PAR_MARK
                .byte   $FF             ; SER_PAR_SPACE

; ------------------------------------------------------------------------
; Addresses

SCCAREG    := $C039
SCCBREG    := $C038
SCCADATA   := $C03B
SCCBDATA   := $C03A

; We're supposed to get SerFlag's address using GetAddr on ROMs 1 and 3.
; (https://archive.org/details/IIgs_2523018_SCC_Access, page 9)
; But, it's the same value as on ROM0. As we don't expect a ROM 4 anytime
; soon with a different value, let's keep it simple.

SER_FLAG   := $E10104

; ------------------------------------------------------------------------
; Channels

CHANNEL_B              = 0
CHANNEL_A              = 1

; ------------------------------------------------------------------------
; Write registers, read registers, and values that interest us

WR_INIT_CTRL           = 0
RR_INIT_STATUS         = 0
INIT_CTRL_CLEAR_EIRQ   = %00010000
INIT_CTRL_CLEAR_ERR    = %00110000
INIT_STATUS_READY      = %00000100
INIT_STATUS_RTS        = %00100000

WR_TX_RX_MODE_CTRL     = 1
TX_RX_MODE_OFF         = %00000000
TX_RX_MODE_RXIRQ       = %00010001

WR_RX_CTRL             = 3              ; (Ref page 5-7)
RR_RX_STATUS           = 9              ; Corresponding status register
RX_CTRL_ON             = %00000001      ; ORed, Rx enabled
RX_CTRL_OFF            = %11111110      ; ANDed,Rx disabled

WR_TX_RX_CTRL          = 4
RR_TX_RX_STATUS        = 4

WR_TX_CTRL             = 5              ; (Ref page 5-9)
RR_TX_STATUS           = 5              ; Corresponding status register
TX_CTRL_ON             = %00001000      ; ORed, Tx enabled
TX_CTRL_OFF            = %11110111      ; ANDed,Tx disabled
TX_DTR_ON              = %01111111      ; ANDed,DTR ON (high)
TX_DTR_OFF             = %10000000      ; ORed, DTR OFF
TX_RTS_ON              = %00000010      ; ORed, RTS ON (low)
TX_RTS_OFF             = %11111101      ; ANDed, RTS OFF

WR_MASTER_IRQ_RST      = 9              ; (Ref page 5-14)
MASTER_IRQ_SHUTDOWN    = %00000010      ; STA'd
MASTER_IRQ_MIE_RST     = %00001010      ; STA'd
MASTER_IRQ_SET         = %00011001      ; STA'd

WR_CLOCK_CTRL          = 11             ; (Ref page 5-17)

WR_BAUDL_CTRL          = 12             ; (Ref page 5-18)
WR_BAUDH_CTRL          = 13             ; (Ref page 5-19)

WR_MISC_CTRL           = 14             ; (Ref page 5-19)

WR_IRQ_CTRL            = 15             ; (Ref page 5-20)
IRQ_CLEANUP_EIRQ       = %00001000

RR_SPEC_COND_STATUS    = 1              ; (Ref page 5-23)
SPEC_COND_FRAMING_ERR  = %01000000
SPEC_COND_OVERRUN_ERR  = %00100000

RR_IRQ_STATUS          = 2              ; (Ref page 5-24)
IRQ_MASQ               = %01110000      ; ANDed
IRQ_RX                 = %00100000
IRQ_SPECIAL            = %01100000

RR_INTR_PENDING_STATUS = 3              ; (Ref page 5-25)
INTR_IS_RX             = %00100100      ; ANDed (RX IRQ, channel A or B)

        .code

; Read register value to A.
; Input:  X as channel
;         Y as register
; Output: A
readSSCReg:
      cpx       #0
      bne       ReadAreg
      sty       SCCBREG
      lda       SCCBREG
      rts
ReadAreg:
      sty       SCCAREG
      lda       SCCAREG
      rts

; Write value of A to a register.
; Input: X as channel
;        Y as register
writeSCCReg:
      cpx       #0
      bne       WriteAreg
      sty       SCCBREG
      sta       SCCBREG
      rts
WriteAreg:
      sty       SCCAREG
      sta       SCCAREG
      rts

;----------------------------------------------------------------------------
; SER_INSTALL: Is called after the driver is loaded into memory. If possible,
; check if the hardware is present. Must return an SER_ERR_xx code in a/x.
;
; Since we don't have to manage the IRQ vector on the Apple II, this is
; actually the same as:
;
; SER_UNINSTALL: Is called before the driver is removed from memory.
; No return code required (the driver is removed from memory on return).
;
; and:
;
; SER_CLOSE: Close the port and disable interrupts. Called without parameters.
; Must return an SER_ERR_xx code in a/x.

SER_INSTALL:
SER_UNINSTALL:
SER_CLOSE:
        ; Check if this is a IIgs (Apple II Miscellaneous TechNote #7,
        ; Apple II Family Identification)
        sec
        bit     $C082
        jsr     $FE1F
        bit     $C080

        bcc     IIgs

        lda     #SER_ERR_NO_DEVICE      ; Not a IIgs
        ldx     #$00                    ; Promote char return value
        rts

IIgs:
        ldx     Opened                  ; Check for open port
        beq     :+

        ldx     Channel

        ; Deactivate interrupts
        sei
        ldy     #WR_MASTER_IRQ_RST
        lda     #MASTER_IRQ_SHUTDOWN
        jsr     writeSCCReg

        ldy     #WR_TX_RX_MODE_CTRL
        lda     #TX_RX_MODE_OFF
        jsr     writeSCCReg

        ; Reset SerFlag to what it was
        lda     SerFlagOrig
        sta     SER_FLAG

        lda     SCCBDATA

        ; Clear external interrupts (twice)
        ldy     #WR_INIT_CTRL
        lda     #INIT_CTRL_CLEAR_EIRQ
        jsr     writeSCCReg
        jsr     writeSCCReg

        ; Reset MIE for firmware use
        ldy     #WR_MASTER_IRQ_RST
        lda     #MASTER_IRQ_MIE_RST
        jsr     writeSCCReg

        ldx     #$00
        stx     Opened                  ; Mark port as closed

        cli
:       txa                             ; Promote char return value
        rts

getClockSource:
        .assert SER_PARAMS::BAUDRATE = 0, error
        lda     (ptr1)                  ; Baudrate index - cc65 value
        cmp     #SER_BAUD_115200
        lda     #$00
        adc     #$00
        sta     CurClockSource          ; 0 = BRG, 1 = RTxC
        rts

;----------------------------------------------------------------------------
; SER_OPEN: A pointer to a ser_params structure is passed in ptr1.
; Must return an SER_ERR_xx code in a/x.

SER_OPEN:
        sei

        ; Check if the handshake setting is valid
        ldy     #SER_PARAMS::HANDSHAKE  ; Handshake
        lda     (ptr1),y
        cmp     #SER_HS_SW              ; Not supported
        beq     InvParam

        sta     HSType                  ; Store flow control type

        ; Initialize buffers
        ldy     #$00
        sty     Stopped
        sty     RecvHead
        sty     RecvTail
        sty     SendHead
        sty     SendTail
        dey                             ; Y = 255
        sty     RecvFreeCnt
        sty     SendFreeCnt

        ldx     Channel

        ldy     #RR_INIT_STATUS         ; Hit rr0 once to sync up
        jsr     readSSCReg

        ldy     #WR_MISC_CTRL           ; WR14: Turn everything off
        lda     #$00
        jsr     writeSCCReg

        jsr     getClockSource          ; Should we use BRG or RTxC?

        ldy     #SER_PARAMS::STOPBITS   ; WR4 setup: clock mult., stop & parity
        lda     (ptr1),y                ; Stop bits
        tay
        lda     StopTable,y             ; Get value

        pha
        ldy     #SER_PARAMS::PARITY
        lda     (ptr1),y                ; Parity bits
        tay
        pla
        ora     ParityTable,y           ; Get value
        bmi     InvParam

        ldy     CurClockSource          ; Clock multiplier
        ora     ClockMultiplier,y

        ldy     #WR_TX_RX_CTRL
        jsr     writeSCCReg             ; End of WR4 setup

        ldy     CurClockSource          ; WR11 setup: clock source
        cpx     #CHANNEL_B
        beq     SetClock
        iny                             ; Shift to get correct ClockSource val
        iny                             ; depending on our channel

SetClock:
        lda     ClockSource,y
        ldy     #WR_CLOCK_CTRL
        jsr     writeSCCReg             ; End of WR11 setup

        lda     ChanIrqFlags,x          ; Store which IRQ bits we'll check
        sta     CurChanIrqFlags

SetBaud:
        .assert SER_PARAMS::BAUDRATE = 0, error
        lda     (ptr1)                  ; Baudrate index - cc65 value
        asl
        tay

        lda     BaudTable,y             ; Get low byte of register value
        bpl     BaudOK                  ; Verify baudrate is supported

InvParam:
        lda     #SER_ERR_INIT_FAILED
        ldy     #$00                    ; Mark port closed
        bra     SetupOut

BaudOK:
        phy                             ; WR12 setup: BRG time constant, low byte
        ldy     #WR_BAUDL_CTRL          ; Setting WR12 & 13 is useless if we're using
        jsr     writeSCCReg             ; RTxC, but doing it anyway makes code smaller
        ply

        iny
        lda     BaudTable,y             ; WR13 setup: BRG time constant, high byte
        ldy     #WR_BAUDH_CTRL
        jsr     writeSCCReg

        ldy     CurClockSource          ; WR14 setup: BRG enabling
        lda     BrgEnabled,y
        ldy     #WR_MISC_CTRL           ; Time to turn this thing on
        jsr     writeSCCReg

        ldy     #SER_PARAMS::DATABITS   ; WR3 setup: RX data bits
        lda     (ptr1),y
        tay
        lda     RxBitTable,y
        ora     #RX_CTRL_ON             ; and turn receiver on

        phy
        ldy     #WR_RX_CTRL
        jsr     writeSCCReg             ; End of WR3 setup
        ply

        lda     TxBitTable,y            ; WR5 setup: TX data bits
        ora     #TX_CTRL_ON             ; and turn transmitter on
        and     #TX_DTR_ON              ; and turn DTR on

        sta     RtsOff                  ; Save value for flow control

        ora     #TX_RTS_ON              ; and turn RTS on

        ldy     #WR_TX_CTRL
        jsr     writeSCCReg             ; End of WR5 setup

        ldy     #WR_IRQ_CTRL            ; WR15 setup: IRQ
        lda     #IRQ_CLEANUP_EIRQ
        jsr     writeSCCReg

        ldy     #WR_INIT_CTRL           ; WR0 setup: clear existing IRQs
        lda     #INIT_CTRL_CLEAR_EIRQ
        jsr     writeSCCReg             ; Clear (write twice)
        jsr     writeSCCReg

        ldy     #WR_TX_RX_MODE_CTRL     ; WR1 setup: Activate RX IRQ
        lda     #TX_RX_MODE_RXIRQ
        jsr     writeSCCReg

        lda     SCCBREG                 ; WR9 setup: Activate master IRQ
        ldy     #WR_MASTER_IRQ_RST
        lda     #MASTER_IRQ_SET
        jsr     writeSCCReg

        lda     SER_FLAG                ; Get SerFlag's current value
        sta     SerFlagOrig             ; and save it

        ora     ChanIrqMask,x           ; Tell firmware which channel IRQs we want
        sta     SER_FLAG

        ldy     #$01                    ; Mark port opened
        lda     #SER_ERR_OK

SetupOut:
        ldx     #$00                    ; Promote char return value
        sty     Opened
        cli
        rts

;----------------------------------------------------------------------------
; SER_GET: Will fetch a character from the receive buffer and store it into the
; variable pointed to by ptr1. If no data is available, SER_ERR_NO_DATA is
; returned.

SER_GET:
        ldx     Channel

        lda     RecvFreeCnt             ; Check for buffer empty
        cmp     #$FF
        beq     NoData

        ldy     Stopped                 ; Check for flow stopped
        beq     :+
        cmp     #63                     ; Enough free?
        bcc     :+
        stz     Stopped                 ; Release flow control

        lda     RtsOff
        ora     #TX_RTS_ON

        ldy     #WR_TX_CTRL
        jsr     writeSCCReg

:       ldy     RecvHead                ; Get byte from buffer
        lda     RecvBuf,y
        inc     RecvHead
        inc     RecvFreeCnt
        sta     (ptr1)
        lda     #SER_ERR_OK
        .assert SER_ERR_OK = 0, error
        tax
        rts
NoData:
        lda     #SER_ERR_NO_DATA
        ldx     #$00                    ; Promote char return value
        rts

;----------------------------------------------------------------------------
; SER_PUT: Output character in A.
; Must return an SER_ERR_xx code in a/x.

SER_PUT:
        ldx     Channel

        ldy     SendFreeCnt             ; Anything to send first?
        iny                             ; Y = $FF?
        beq     :+
        pha
        lda     #$00                    ; TryHard = false
        jsr     TryToSend
        pla

:       ldy     SendFreeCnt             ; Do we have room to store byte?
        bne     :+
        lda     #SER_ERR_OVERFLOW
        ldx     #$00
        rts

:       ldy     SendTail                ; Put byte into send buffer & send
        sta     SendBuf,y
        inc     SendTail
        dec     SendFreeCnt
        lda     #$FF                    ; TryHard = true
        jsr     TryToSend
        lda     #SER_ERR_OK
        .assert SER_ERR_OK = 0, error
        tax
        rts

;----------------------------------------------------------------------------
; SER_STATUS: Return the status in the variable pointed to by ptr1.
; Must return an SER_ERR_xx code in a/x.
; We provide the read register 0, containing interesting info like
; INIT_STATUS_READY (hardware handshake status) or INIT_STATUS_RTS
; (ready to send).

SER_STATUS:
        ldx     Channel
        ldy     #RR_INIT_STATUS
        jsr     readSSCReg
        ldx     #$00
        sta     (ptr1)
        .assert SER_ERR_OK = 0, error
        txa
        rts

;----------------------------------------------------------------------------
; SER_IOCTL: Driver defined entry point. The wrapper will pass a pointer to ioctl
; specific data in ptr1, and the ioctl code in A.
; Sets communication channel A or B (A = 1, B = 0)
; Must return an SER_ERR_xx code in a/x.

SER_IOCTL:
        ora     ptr1+1                  ; Check data msb and code to be 0
        bne     :+

        ldx     ptr1                    ; Check data lsb to be 0 or 1
        bmi     :+
        cpx     #$02
        bcs     :+

        stx     Channel
        .assert SER_ERR_OK = 0, error
        tax
        rts

:       lda     #SER_ERR_INV_IOCTL
        ldx     #$00                    ; Promote char return value
        rts

;----------------------------------------------------------------------------
; SER_IRQ: Called from the builtin runtime IRQ handler as a subroutine. All
; registers are already saved, no parameters are passed, but the carry flag
; is clear on entry. The routine must return with carry set if the interrupt
; was handled, otherwise with carry clear.

SER_IRQ:
        ldy     #RR_INTR_PENDING_STATUS ; IRQ status is always in A reg
        sty     SCCAREG
        lda     SCCAREG

        and     CurChanIrqFlags         ; Is this ours?
        beq     Done

        and     #INTR_IS_RX             ; Is this an RX irq?
        beq     CheckSpecial

        ldx     Channel
        beq     ReadBdata
        lda     SCCADATA
        bra     ReadDone
ReadBdata:
        lda     SCCBDATA                ; Get byte
ReadDone:
        ldx     RecvFreeCnt             ; Check if we have free space left
        beq     Flow                    ; Jump if no space in receive buffer
        ldy     RecvTail                ; Load buffer pointer
        sta     RecvBuf,y               ; Store received byte in buffer
        inc     RecvTail                ; Increment buffer pointer
        dec     RecvFreeCnt             ; Decrement free space counter
        cpx     #33
        bcc     Flow                    ; Assert flow control if buffer space low
        rts                             ; Interrupt handled (carry already set)

CheckSpecial:
        ; Always check IRQ special flags from Channel B (Ref page 5-24)
        ldy     #RR_IRQ_STATUS
        sty     SCCBREG
        lda     SCCBREG

        and     #IRQ_MASQ
        cmp     #IRQ_SPECIAL
        beq     Special

        ; Clear exint
        ldx     Channel
        ldy     #WR_INIT_CTRL
        lda     #INIT_CTRL_CLEAR_EIRQ
        jsr     writeSCCReg

        sec
        rts

Flow:   lda     HSType                  ; Don't touch if no flow control
        beq     IRQDone

        ldx     Channel                 ; Assert flow control if buffer space too low
        ldy     #WR_TX_CTRL
        lda     RtsOff
        jsr     writeSCCReg

        sta     Stopped
IRQDone:sec                             ; Interrupt handled
Done:   rts

Special:ldx     Channel
        ldy     #RR_SPEC_COND_STATUS
        jsr     readSSCReg

        tax
        and     #SPEC_COND_FRAMING_ERR
        bne     BadChar
        txa
        and     #SPEC_COND_OVERRUN_ERR
        beq     BadChar

        ldy     #WR_INIT_CTRL
        lda     #INIT_CTRL_CLEAR_ERR
        jsr     writeSCCReg

        sec
        rts

BadChar:
        cpx     #CHANNEL_B
        beq     BadCharB
        lda     SCCADATA
        bra     BadCharDone
BadCharB:
        lda     SCCBDATA                ; Remove char in error
BadCharDone:
        sec
        rts

;----------------------------------------------------------------------------
; Try to send a byte. Internal routine. A = TryHard, X = Channel

TryToSend:
        sta     tmp1                    ; Remember tryHard flag
Again:  lda     SendFreeCnt             ; Anything to send?
        cmp     #$FF
        beq     Quit                    ; No

        lda     Stopped                 ; Check for flow stopped
        bne     Quit                    ; Bail out if it is

Wait:
        ldy     #RR_INIT_STATUS
        jsr     readSSCReg              ; Check that we're ready to send
        tay
        and     #INIT_STATUS_READY
        beq     NotReady

        tya
        and     #INIT_STATUS_RTS        ; Ready to send
        bne     Send

NotReady:
        bit     tmp1                    ; Keep trying if must try hard
        bmi     Wait
Quit:   rts

Send:   ldy     SendHead                ; Send byte
        lda     SendBuf,y

        cpx     #CHANNEL_B
        beq     WriteBdata
        sta     SCCADATA
        bra     WriteDone
WriteBdata:
        sta     SCCBDATA
WriteDone:
        inc     SendHead
        inc     SendFreeCnt
        jmp     Again                   ; Continue flushing TX buffer
