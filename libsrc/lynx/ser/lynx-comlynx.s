;
; Serial driver for the Atari Lynx ComLynx port.
;
; Karri Kaksonen, 17.09.2009
;

        .include        "lynx.inc"
        .include        "zeropage.inc"
        .include        "ser-kernel.inc"
        .include        "ser-error.inc"

        .macpack        module


; ------------------------------------------------------------------------
; Header. Includes jump table

        module_header   _lynx_comlynx_ser

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
;

        .bss

TxBuffer:       .res    256
RxBuffer:       .res    256
RxPtrIn:        .res    1
RxPtrOut:       .res    1
TxPtrIn:        .res    1
TxPtrOut:       .res    1
contrl:         .res    1
SerialStat:     .res    1
TxDone:         .res    1

        .code

;----------------------------------------------------------------------------
; SER_INSTALL: Is called after the driver is loaded into memory.
;
; Must return an SER_ERR_xx code in a/x.

SER_INSTALL:
        ; Set up IRQ vector ?

;----------------------------------------------------------------------------
; SER_UNINSTALL: Is called before the driver is removed from memory.
; No return code required (the driver is removed from memory on return).
;

SER_UNINSTALL:

;----------------------------------------------------------------------------
; SER_CLOSE: Close the port and disable interrupts. Called without parameters.
; Must return an SER_ERR_xx code in a/x.

SER_CLOSE:
        ; Disable interrupts and stop timer 4 (serial)
        lda     #$0C  ; TXOPEN|RESETERR
        sta     SERCTL
        lda     #$00  ; Disable count and no reload
        sta     TIM4CTLA

        ; Done, return an error code
        lda     #SER_ERR_OK
        .assert SER_ERR_OK = 0, error
        tax
        rts

;----------------------------------------------------------------------------
; SER_OPEN: A pointer to a ser_params structure is passed in ptr1.
;
; The Lynx has only two correct serial data formats:
; 8 bits, parity mark, 1 stop bit
; 8 bits, parity space, 1 stop bit
;
; It also has two wrong formats;
; 8 bits, even parity, 1 stop bit
; 8 bits, odd parity, 1 stop bit
;
; Unfortunately the parity bit includes itself in the calculation making
; parity not compatible with the rest of the world.
;
; We can only specify a few baud rates.
; Lynx has two non-standard speeds 31250 and 62500 which are
; frequently used in games.
;
; The receiver will always read the parity and report parity errors.
;
; Must return an SER_ERR_xx code in a/x.

SER_OPEN:
        stz     RxPtrIn
        stz     RxPtrOut
        stz     TxPtrIn
        stz     TxPtrOut

        ; source period is 1 us
        lda     #%00011000
        sta     TIM4CTLA
        ldy     #SER_PARAMS::BAUDRATE
        lda     (ptr1),y

        ldx     #1
        cmp     #SER_BAUD_62500
        beq     setbaudrate

        ldx     #3
        cmp     #SER_BAUD_31250
        beq     setbaudrate

        ldx     #12
        cmp     #SER_BAUD_9600
        beq     setbaudrate

        ldx     #25
        cmp     #SER_BAUD_4800
        beq     setbaudrate

        ldx     #51
        cmp     #SER_BAUD_2400
        beq     setbaudrate

        ldx     #103
        cmp     #SER_BAUD_1200
        beq     setbaudrate

        ldx     #207
        cmp     #SER_BAUD_600
        beq     setbaudrate

        ; clock = 6 * 15625
        ldx     #%00011010
        stx     TIM4CTLA

        ldx     #12
        cmp     #SER_BAUD_7200
        beq     setbaudrate

        ldx     #25
        cmp     #SER_BAUD_3600
        beq     setbaudrate

        ldx     #207
        stx     TIM4BKUP

        ; clock = 4 * 15625
        ldx     #%00011100
        cmp     #SER_BAUD_300
        beq     setprescaler

        ; clock = 6 * 15625
        ldx     #%00011110
        cmp     #SER_BAUD_150
        beq     setprescaler

        ; clock = 1 * 15625
        ldx     #%00011111
        stx     TIM4CTLA
        cmp     #SER_BAUD_75
        beq     baudsuccess

        ldx     #141
        cmp     #SER_BAUD_110
        beq     setbaudrate

        ; clock = 2 * 15625
        ldx     #%00011010
        stx     TIM4CTLA
        ldx     #68
        cmp     #SER_BAUD_1800
        beq     setbaudrate

        ; clock = 6 * 15625
        ldx     #%00011110
        stx     TIM4CTLA
        ldx     #231
        cmp     #SER_BAUD_134_5
        beq     setbaudrate

        lda     #SER_ERR_BAUD_UNAVAIL
        ldx     #0 ; return value is char
        rts

setprescaler:
        stx     TIM4CTLA
        bra     baudsuccess
setbaudrate:
        stx     TIM4BKUP
baudsuccess:
        ldx     #TXOPEN|PAREVEN
        stx     contrl
        ldy     #SER_PARAMS::DATABITS   ; Databits
        lda     (ptr1),y
        cmp     #SER_BITS_8
        bne     invparameter
        ldy     #SER_PARAMS::STOPBITS   ; Stopbits
        lda     (ptr1),y
        cmp     #SER_STOP_1
        bne     invparameter
        ldy     #SER_PARAMS::PARITY     ; Parity
        lda     (ptr1),y
        cmp     #SER_PAR_NONE
        beq     invparameter
        cmp     #SER_PAR_MARK
        beq     checkhs
        cmp     #SER_PAR_SPACE
        bne     @L0
        ldx     #TXOPEN
        stx     contrl
        bra     checkhs
@L0:
        ldx     #PAREN|TXOPEN|PAREVEN
        stx     contrl
        cmp     #SER_PAR_EVEN
        beq     checkhs
        ldx     #PAREN|TXOPEN
        stx     contrl
checkhs:
        ldx     contrl
        stx     SERCTL
        ldy     #SER_PARAMS::HANDSHAKE  ; Handshake
        lda     (ptr1),y
        cmp     #SER_HS_NONE
        bne     invparameter
        lda     SERDAT
        lda     contrl
        ora     #RXINTEN|RESETERR       ; Turn on interrupts for receive
        sta     SERCTL
        lda     #SER_ERR_OK
        .assert SER_ERR_OK = 0, error
        tax
        rts
invparameter:
        lda     #SER_ERR_INIT_FAILED
        ldx     #0 ; return value is char
        rts

;----------------------------------------------------------------------------
; SER_GET: Will fetch a character from the receive buffer and store it into the
; variable pointed to by ptr1. If no data is available, SER_ERR_NO_DATA is
; returned.

SER_GET:
        lda     RxPtrIn
        cmp     RxPtrOut
        bne     GetByte
        lda     #SER_ERR_NO_DATA
        ldx     #0 ; return value is char
        rts
GetByte:
        ldy     RxPtrOut
        lda     RxBuffer,y
        inc     RxPtrOut
        ldx     #$00
        sta     (ptr1,x)
        txa                     ; Return code = 0
        rts

;----------------------------------------------------------------------------
; SER_PUT: Output character in A.
; Must return an SER_ERR_xx code in a/x.

SER_PUT:
        tax
        lda     TxPtrIn
        ina
        cmp     TxPtrOut
        bne     PutByte

        lda     #SER_ERR_OVERFLOW
        ldx     #0 ; return value is char
        rts

PutByte:
        ldy     TxPtrIn
        txa
        sta     TxBuffer,y
        inc     TxPtrIn

        bit     TxDone      ; Check bit 7 of TxDone (TXINTEN)
        bmi     @L1         ; Was TXINTEN already set?
        php
        sei
        lda     contrl      ; contrl does not include RXINTEN setting
        ora     #TXINTEN|RESETERR
        sta     SERCTL      ; Allow TX-IRQ to hang RX-IRQ (no receive while transmitting)
        sta     TxDone
        plp                 ; Restore processor and interrupt enable
@L1:
        lda     #SER_ERR_OK
        .assert SER_ERR_OK = 0, error
        tax
        rts

;----------------------------------------------------------------------------
; SER_STATUS: Return the status in the variable pointed to by ptr1.
; Must return an SER_ERR_xx code in a/x.

SER_STATUS:
        lda     SerialStat
        ldx     #$00
        sta     (ptr1,x)
        txa                     ; Return code = 0
        rts

;----------------------------------------------------------------------------
; SER_IOCTL: Driver defined entry point. The wrapper will pass a pointer to ioctl
; specific data in ptr1, and the ioctl code in A.
; Must return an SER_ERR_xx code in a/x.

SER_IOCTL:
        lda     #SER_ERR_INV_IOCTL
        ldx     #0 ; return value is char
        rts

;----------------------------------------------------------------------------
; SER_IRQ: Called from the builtin runtime IRQ handler as a subroutine. All
; registers are already saved, no parameters are passed, but the carry flag
; is clear on entry. The routine must return with carry set if the interrupt
; was handled, otherwise with carry clear.
;
; Both the Tx and Rx interrupts are level sensitive instead of edge sensitive.
; Due to this bug you have to disable the interrupt before clearing it.

SER_IRQ:
        lda     INTSET          ; Poll all pending interrupts
        and     #SERIAL_INTERRUPT
        bne     @L0
        clc
        rts
@L0:
        bit     TxDone
        bmi     @tx_irq     ; Transmit in progress

        ldx     SERDAT      ; Read received data
        lda     contrl
        and     #PAREN      ; Parity enabled implies SER_PAR_EVEN or SER_PAR_ODD
        ora 	#OVERRUN|FRAMERR|RXBRK
        bit     SERCTL      ; Compare with SERCTL

        beq     @rx_irq     ; No errors so far

        tsb     SerialStat  ; Save error condition
        bit     #RXBRK      ; Check for break signal
        beq     @noBreak

        stz     TxPtrIn     ; Break received - drop buffers
        stz     TxPtrOut
        stz     RxPtrIn
        stz     RxPtrOut
@noBreak:
        lda     contrl
        ora     #RXINTEN|RESETERR
        sta     SERCTL
        bra     @IRQexit

@rx_irq:
        lda     contrl
        ora     #RXINTEN|RESETERR
        sta     SERCTL
        txa
        ldx     RxPtrIn
        sta     RxBuffer,x
        txa
        inx

        cpx     RxPtrOut
        beq     @1
        stx     RxPtrIn
        bra     @IRQexit

@1:
        sta     RxPtrIn
        lda     #$80
        tsb     SerialStat
        lda     contrl
        ora     #RXINTEN|RESETERR
        sta     SERCTL
        bra     @IRQexit

@tx_irq:
        ldx     TxPtrOut    ; Have all bytes been sent?
        cpx     TxPtrIn
        beq     @allSent

        lda     TxBuffer,x  ; Send next byte
        sta     SERDAT
        inc     TxPtrOut

@exit1:
        lda     contrl
        ora     #TXINTEN|RESETERR
        sta     SERCTL
        bra     @IRQexit

@allSent:
        lda     SERCTL       ; All bytes sent
        bit     #TXEMPTY
        beq     @exit1
        bvs     @exit1
        stz     TxDone
        lda     contrl
        ora     #RXINTEN|RESETERR   ; Re-enable receive interrupt
        sta     SERCTL

@IRQexit:
        lda     #SERIAL_INTERRUPT
        sta     INTRST
        clc
        rts
