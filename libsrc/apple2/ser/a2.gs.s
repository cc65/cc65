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

RecvBuf:        .res    256             ; Receive buffers: 256 bytes
SendBuf:        .res    256             ; Send buffers: 256 bytes

        .data

Slot:           .byte   $00             ; 2 when opened
Channel:        .byte   $00             ; Channel B by default
CurChanIrqFlags:.byte   INTR_PENDING_RX_EXT_B

SerFlagOrig:    .byte   $00

; Tables used to translate cc65 RS232 params into register values
; (Ref page 5-18 and 5-19)
BaudLowTable:   .byte   $7E             ; SER_BAUD_300
                .byte   $5E             ; SER_BAUD_1200
                .byte   $2E             ; SER_BAUD_2400
                .byte   $16             ; SER_BAUD_4800
                .byte   $0A             ; SER_BAUD_9600
                .byte   $04             ; SER_BAUD_19200
                .byte   $01             ; SER_BAUD_38400
                .byte   $00             ; SER_BAUD_57600

BaudHighTable:  .byte   $01             ; SER_BAUD_300
                .byte   $00             ; SER_BAUD_1200
                .byte   $00             ; SER_BAUD_2400
                .byte   $00             ; SER_BAUD_4800
                .byte   $00             ; SER_BAUD_9600
                .byte   $00             ; SER_BAUD_19200
                .byte   $00             ; SER_BAUD_38400
                .byte   $00             ; SER_BAUD_57600

RxBitTable:     .byte   %00000000       ; SER_BITS_5, in WR_RX_CTRL (WR3)
                .byte   %10000000       ; SER_BITS_6  (Ref page 5-7)
                .byte   %01000000       ; SER_BITS_7
                .byte   %11000000       ; SER_BITS_8

TxBitTable:     .byte   %00000000       ; SER_BITS_5, in WR_TX_CTRL (WR5)
                .byte   %01000000       ; SER_BITS_6  (Ref page 5-9)
                .byte   %00100000       ; SER_BITS_7
                .byte   %01100000       ; SER_BITS_8

        .rodata

BaudTable:                              ; bit7 = 1 means setting is invalid
                                        ; Otherwise refers to the index in
                                        ; Baud(Low/High)Table
                .byte   $FF             ; SER_BAUD_45_5
                .byte   $FF             ; SER_BAUD_50
                .byte   $FF             ; SER_BAUD_75
                .byte   $FF             ; SER_BAUD_110
                .byte   $FF             ; SER_BAUD_134_5
                .byte   $FF             ; SER_BAUD_150
                .byte   $00             ; SER_BAUD_300
                .byte   $FF             ; SER_BAUD_600
                .byte   $01             ; SER_BAUD_1200
                .byte   $FF             ; SER_BAUD_1800
                .byte   $02             ; SER_BAUD_2400
                .byte   $FF             ; SER_BAUD_3600
                .byte   $03             ; SER_BAUD_4800
                .byte   $FF             ; SER_BAUD_7200
                .byte   $04             ; SER_BAUD_9600
                .byte   $05             ; SER_BAUD_19200
                .byte   $06             ; SER_BAUD_38400
                .byte   $07             ; SER_BAUD_57600
                .byte   $FF             ; SER_BAUD_115200
                .byte   $FF             ; SER_BAUD_230400

StopTable:      .byte   %00000100       ; SER_STOP_1, in WR_TX_RX_CTRL (WR4)
                .byte   %00001100       ; SER_STOP_2  (Ref page 5-8)

ParityTable:    .byte   %00000000       ; SER_PAR_NONE, in WR_TX_RX_CTRL (WR4)
                .byte   %00000001       ; SER_PAR_ODD   (Ref page 5-8)
                .byte   %00000011       ; SER_PAR_EVEN
                .byte   $FF             ; SER_PAR_MARK
                .byte   $FF             ; SER_PAR_SPACE

IdOfsTable:     .byte   $00             ; First firmware instruction
                .byte   $05             ; Pascal 1.0 ID byte
                .byte   $07             ; Pascal 1.0 ID byte
                .byte   $0B             ; Pascal 1.1 generic signature byte
                .byte   $0C             ; Device signature byte

IdValTable:     .byte   $E2             ; SEP instruction
                .byte   $38             ; Fixed
                .byte   $18             ; Fixed
                .byte   $01             ; Fixed
                .byte   $31             ; Serial or parallel I/O card type 1

IdTableLen      = * - IdValTable

; ------------------------------------------------------------------------
; Addresses

ZILOG_BASE := $C200

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
TX_RX_CLOCK_MUL        = %01000000      ; Clock x16 (Ref page 5-8)

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
CLOCK_CTRL_CH_A        = %11010000
CLOCK_CTRL_CH_B        = %01010000

WR_BAUDL_CTRL          = 12             ; (Ref page 5-18)
WR_BAUDH_CTRL          = 13             ; (Ref page 5-19)

WR_MISC_CTRL           = 14             ; (Ref page 5-19)
MISC_CTRL_RATE_GEN_ON  = %00000001      ; ORed
MISC_CTRL_RATE_GEN_OFF = %11111110      ; ANDed

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
INTR_PENDING_RX_EXT_A  = %00101000      ; ANDed (RX or special IRQ)
INTR_PENDING_RX_EXT_B  = %00000101      ; ANDed (RX or special IRQ)
INTR_IS_RX             = %00100100      ; ANDed (RX IRQ, channel A or B)

SER_FLAG_CH_A          = %00111000
SER_FLAG_CH_B          = %00000111

        .code

; Read a register
; Input: X as channel
; Output result in A
.macro rra      In,Reg
       lda      Reg
       sta      In,x
       lda      In,x
.endmacro

; Write value of A to a register.
; Input: X as channel
.macro wra      Out,Reg
       pha
       lda      Reg
       sta      Out,x
       pla
       sta      Out,x
.endmacro

; Write value passed as parameter to a register.
; Input: X as channel
.macro wrr      Out,Reg,Val
       lda      Reg
       sta      Out,x
       lda      Val
       sta      Out,x
.endmacro

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
        ldx     Slot                    ; Check for open port
        beq     :+
        ldx     Channel

        ; Deactivate interrupts
        sei
        wrr     SCCBREG, #WR_MASTER_IRQ_RST, #MASTER_IRQ_SHUTDOWN
        wrr     SCCBREG, #WR_TX_RX_MODE_CTRL, #TX_RX_MODE_OFF

        ; Reset SerFlag to what it was
        lda     SerFlagOrig
        sta     SER_FLAG

        lda     SCCBDATA

        ; Clear external interrupts (twice)
        ldy     #WR_INIT_CTRL
        lda     #INIT_CTRL_CLEAR_EIRQ

        sty     SCCBREG
        sta     SCCBREG
        sty     SCCBREG
        sta     SCCBREG

        ; Reset MIE for firmware use
        wrr     SCCBREG, #WR_MASTER_IRQ_RST, #MASTER_IRQ_MIE_RST

        ldx     #$00
        stx     Slot                    ; Mark port as closed

        cli
:       txa
        rts

;----------------------------------------------------------------------------
; SER_OPEN: A pointer to a ser_params structure is passed in ptr1.
; Must return an SER_ERR_xx code in a/x.

SER_OPEN:
        ; Check Pascal 1.1 Firmware Protocol ID bytes
        ldx     #$00
Check:  ldy     IdOfsTable,x
        lda     IdValTable,x
        cmp     ZILOG_BASE,y
        bne     NoDevice
        inx
        cpx     #IdTableLen
        bcc     Check

        beq     HardwareFound

        ; Device (hardware) not found
NoDevice:
        lda     #SER_ERR_NO_DEVICE
SetupErrOut:
        cli
        ldx     #$00                    ; Promote char return value
        stx     Slot                    ; Mark port closed
        rts

HardwareFound:
        ; Check if the handshake setting is valid
        ldy     #SER_PARAMS::HANDSHAKE  ; Handshake
        lda     (ptr1),y
        cmp     #SER_HS_HW              ; This is all we support
        beq     SetupBufs

InvParam:
        lda     #SER_ERR_INIT_FAILED
        jmp     SetupErrOut

SetupBufs:
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

        rra     SCCBREG,#$00            ; Hit rr0 once to sync up

        ldy     #SER_PARAMS::STOPBITS
        lda     (ptr1),y                ; Stop bits
        tay
        lda     StopTable,y             ; Get value

        pha
        ldy     #SER_PARAMS::PARITY
        lda     (ptr1),y                ; Parity bits
        tay
        cmp     #$FF
        beq     InvParam
        pla
        ora     ParityTable,y           ; Get value

        ora     #TX_RX_CLOCK_MUL

        wra     SCCBREG,#WR_TX_RX_CTRL

        cpx     #$00
        bne     ClockA
ClockB:
        wrr     SCCBREG,#WR_CLOCK_CTRL,#CLOCK_CTRL_CH_B

        lda     #INTR_PENDING_RX_EXT_B  ; Store which IRQ bits we'll check
        sta     CurChanIrqFlags

        bra     SetBaud
ClockA:
        wrr     SCCBREG,#WR_CLOCK_CTRL,#CLOCK_CTRL_CH_A

        lda     #INTR_PENDING_RX_EXT_A  ; Store which IRQ bits we'll check
        sta     CurChanIrqFlags

SetBaud:
        ldy     #SER_PARAMS::BAUDRATE
        lda     (ptr1),y                ; Baudrate index - cc65 value
        tay

        lda     BaudTable,y             ; Get chip value from Low/High tables
        tay

        lda     BaudLowTable,y          ; Get low byte
        bmi     InvParam                ; Branch if rate not supported

        wra     SCCBREG,#WR_BAUDL_CTRL

        lda     BaudHighTable,y         ; Get high byte
        wra     SCCBREG,#WR_BAUDH_CTRL

        lda     #$00
        wra     SCCBREG,#WR_MISC_CTRL

        ora     #MISC_CTRL_RATE_GEN_ON  ; Time to turn this thing on
        wra     SCCBREG,#WR_MISC_CTRL

        ; Final write to RX_CTRL
        ldy     #SER_PARAMS::DATABITS
        lda     (ptr1),y                ; Data bits
        tay
        lda     RxBitTable,y            ; Data bits for RX
        ora     #RX_CTRL_ON             ; Plus turn on
        wra     SCCBREG,#WR_RX_CTRL

        lda     TxBitTable,y            ; Data bits for TX
        ora     #TX_CTRL_ON             ; Plus turn on
        and     #TX_DTR_ON

        sta     RtsOff                  ; Save value for flow control

        ora     #TX_RTS_ON
        wra     SCCBREG,#WR_TX_CTRL

        wrr     SCCBREG,#WR_IRQ_CTRL,#IRQ_CLEANUP_EIRQ

        lda     #WR_INIT_CTRL           ; Clear ext status (write twice)
        sta     SCCBREG,x
        lda     #INIT_CTRL_CLEAR_EIRQ
        sta     SCCBREG,x

        lda     #WR_INIT_CTRL
        sta     SCCBREG,x
        lda     #INIT_CTRL_CLEAR_EIRQ
        sta     SCCBREG,x

        ; Activate RX IRQ
        wrr     SCCBREG,#WR_TX_RX_MODE_CTRL,#TX_RX_MODE_RXIRQ

        wrr     SCCBREG,#WR_MASTER_IRQ_RST,#MASTER_IRQ_SET

        lda     SER_FLAG                ; Get SerFlag's current value
        sta     SerFlagOrig             ; and save it

        cpx     #$00
        bne     IntA
IntB:
        ora     #SER_FLAG_CH_B          ; Inform firmware we want channel B IRQs
        bra     StoreFlag
IntA:
        ora     #SER_FLAG_CH_A          ; Inform firmware we want channel A IRQs
StoreFlag:
        sta     SER_FLAG

        ldy     #$02                    ; Mark port opened
        sty     Slot
        cli
        lda     #SER_ERR_OK
        .assert SER_ERR_OK = 0, error
        tax
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
        wra     SCCBREG,#WR_TX_CTRL

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
        lda     SCCBREG,x
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
        ldx     #$00                    ; IRQ status is always in A reg
        rra     SCCAREG,#RR_INTR_PENDING_STATUS
        and     CurChanIrqFlags         ; Is this ours?
        beq     Done

        and     #INTR_IS_RX             ; Is this an RX irq?
        beq     CheckSpecial

        ldx     Channel
        lda     SCCBDATA,x              ; Get byte
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
        ; X is still 0 there.
        rra     SCCBREG,#RR_IRQ_STATUS

        and     #IRQ_MASQ
        cmp     #IRQ_SPECIAL
        beq     Special

        ; Clear exint
        ldx     Channel
        wrr     SCCBREG,#WR_INIT_CTRL,#INIT_CTRL_CLEAR_EIRQ
        sec
        rts

Flow:   ldx     Channel                 ; Assert flow control if buffer space too low
        lda     RtsOff
        wra     SCCBREG,#WR_TX_CTRL
        sta     Stopped
        sec                             ; Interrupt handled
Done:   rts

Special:
        rra     SCCBREG,#RR_SPEC_COND_STATUS
        tax
        and     #SPEC_COND_FRAMING_ERR
        bne     BadChar
        txa
        and     #SPEC_COND_OVERRUN_ERR
        beq     BadChar

        wrr     SCCBREG,#WR_INIT_CTRL,#INIT_CTRL_CLEAR_ERR
        sec
        rts

BadChar:
        lda     SCCBDATA,x              ; Remove char in error
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

        lda     SCCBREG,x               ; Check that we're ready to send
        tay
        and     #INIT_STATUS_READY
        bne     Send
        tya
        and     #INIT_STATUS_RTS        ; Ready to send
        bit     tmp1                    ; Keep trying if must try hard
        bmi     Again
Quit:   rts

Send:   ldy     SendHead                ; Send byte
        lda     SendBuf,y

        sta     SCCBDATA,x

        inc     SendHead
        inc     SendFreeCnt
        jmp     Again                   ; Continue flushing TX buffer
