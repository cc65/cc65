;
; Serial driver for the Apple II Super Serial Card.
;
; Oliver Schmidt, 21.04.2005
;
; The driver is based on the cc65 rs232 module, which in turn is based on
; Craig Bruce device driver for the Switftlink/Turbo-232.
;
; SwiftLink/Turbo-232 v0.90 device driver, by Craig Bruce, 14-Apr-1998.
;
; This software is Public Domain.  It is in Buddy assembler format.
;
; This device driver uses the SwiftLink RS-232 Serial Cartridge, available from
; Creative Micro Designs, Inc, and also supports the extensions of the Turbo232
; Serial Cartridge.  Both devices are based on the 6551 ACIA chip.  It also
; supports the "hacked" SwiftLink with a 1.8432 MHz crystal.
;
; The code assumes that the kernal + I/O are in context.  On the C128, call
; it from Bank 15.  On the C64, don't flip out the Kernal unless a suitable
; NMI catcher is put into the RAM under then Kernal.  For the SuperCPU, the
; interrupt handling assumes that the 65816 is in 6502-emulation mode.
;

        .include        "zeropage.inc"
        .include        "ser-kernel.inc"
        .include        "ser-error.inc"

        .macpack        module
        .macpack        cpu

; ------------------------------------------------------------------------
; Header. Includes jump table

        .ifdef  __APPLE2ENH__
        module_header   _a2e_ssc_ser
        .else
        module_header   _a2_ssc_ser
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
; I/O definitions

.if (.cpu .bitand CPU_ISET_65C02)
ACIA            = $C088
.else
Offset          = $8F           ; Move 6502 false read out of I/O to page $BF
ACIA            = $C088-Offset
.endif

ACIA_DATA       = ACIA+0        ; Data register
ACIA_STATUS     = ACIA+1        ; Status register
ACIA_CMD        = ACIA+2        ; Command register
ACIA_CTRL       = ACIA+3        ; Control register

;----------------------------------------------------------------------------
; Global variables

        .bss

RecvHead:       .res    1       ; Head of receive buffer
RecvTail:       .res    1       ; Tail of receive buffer
RecvFreeCnt:    .res    1       ; Number of bytes in receive buffer
SendHead:       .res    1       ; Head of send buffer
SendTail:       .res    1       ; Tail of send buffer
SendFreeCnt:    .res    1       ; Number of bytes in send buffer

Stopped:        .res    1       ; Flow-stopped flag
RtsOff:         .res    1       ;

RecvBuf:        .res    256     ; Receive buffers: 256 bytes
SendBuf:        .res    256     ; Send buffers: 256 bytes

Index:          .res    1       ; I/O register index

        .data

Slot:   .byte   $02             ; Default to SSC in slot 2

        .rodata

        ; Tables used to translate RS232 params into register values
BaudTable:                      ; bit7 = 1 means setting is invalid
        .byte   $FF             ; SER_BAUD_45_5
        .byte   $01             ; SER_BAUD_50
        .byte   $02             ; SER_BAUD_75
        .byte   $03             ; SER_BAUD_110
        .byte   $04             ; SER_BAUD_134_5
        .byte   $05             ; SER_BAUD_150
        .byte   $06             ; SER_BAUD_300
        .byte   $07             ; SER_BAUD_600
        .byte   $08             ; SER_BAUD_1200
        .byte   $09             ; SER_BAUD_1800
        .byte   $0A             ; SER_BAUD_2400
        .byte   $0B             ; SER_BAUD_3600
        .byte   $0C             ; SER_BAUD_4800
        .byte   $0D             ; SER_BAUD_7200
        .byte   $0E             ; SER_BAUD_9600
        .byte   $0F             ; SER_BAUD_19200
        .byte   $FF             ; SER_BAUD_38400
        .byte   $FF             ; SER_BAUD_57600
        .byte   $FF             ; SER_BAUD_115200
        .byte   $FF             ; SER_BAUD_230400
BitTable:
        .byte   $60             ; SER_BITS_5
        .byte   $40             ; SER_BITS_6
        .byte   $20             ; SER_BITS_7
        .byte   $00             ; SER_BITS_8
StopTable:
        .byte   $00             ; SER_STOP_1
        .byte   $80             ; SER_STOP_2
ParityTable:
        .byte   $00             ; SER_PAR_NONE
        .byte   $20             ; SER_PAR_ODD
        .byte   $60             ; SER_PAR_EVEN
        .byte   $A0             ; SER_PAR_MARK
        .byte   $E0             ; SER_PAR_SPACE

        ; Check five bytes at known positions on the
        ; slot's firmware to make sure this is an SSC
        ; (or Apple //c comm port) firmware that drives
        ; an ACIA 6551 chip.
        ;
        ; The SSC firmware and the Apple //c(+) comm
        ; port firmware all begin with a BIT instruction.
        ; The IIgs, on the other hand, has a
        ; Zilog Z8530 chip and its firmware starts with
        ; a SEP instruction. We don't want to load this
        ; driver on the IIgs' serial port. We'll
        ; differentiate the firmware on this byte.
        ;
        ; The next four bytes we check are the Pascal
        ; Firmware Protocol Bytes that identify a
        ; serial card. Those are the same bytes for
        ; SSC firmwares, Apple //c firmwares and IIgs
        ; Zilog Z8530 firmwares - which is the reason
        ; we have to check for the firmware's first
        ; instruction too.

IdOfsTable:
        .byte   $00             ; First instruction
        .byte   $05             ; Pascal 1.0 ID byte
        .byte   $07             ; Pascal 1.0 ID byte
        .byte   $0B             ; Pascal 1.1 generic signature byte
        .byte   $0C             ; Device signature byte
IdValTable:
        .byte   $2C             ; BIT
        .byte   $38             ; ID Byte 0 (from Pascal 1.0), fixed
        .byte   $18             ; ID Byte 1 (from Pascal 1.0), fixed
        .byte   $01             ; Generic signature for Pascal 1.1, fixed
        .byte   $31             ; Device signature byte (serial or
                                ; parallel I/O card type 1)

IdTableLen      = * - IdValTable

        .code

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
        ldx     Index           ; Check for open port
        beq     :+

        ; Deactivate DTR and disable 6551 interrupts
        lda     #%00001010
        sta     ACIA_CMD,x

        ; Done, return an error code
:       lda     #SER_ERR_OK
        .assert SER_ERR_OK = 0, error
        tax
        stx     Index           ; Mark port as closed
        rts

;----------------------------------------------------------------------------
; SER_OPEN: A pointer to a ser_params structure is passed in ptr1.
; Must return an SER_ERR_xx code in a/x.

SER_OPEN:
        ldx     #<$C000
        stx     ptr2
        lda     #>$C000
        ora     Slot
        sta     ptr2+1

        ; Check Pascal 1.1 Firmware Protocol ID bytes
:       ldy     IdOfsTable,x
        lda     IdValTable,x
        cmp     (ptr2),y
        bne     NoDevice
        inx
        cpx     #IdTableLen
        bcc     :-

        ; Convert slot to I/O register index
        lda     Slot
        asl
        asl
        asl
        asl
.if .not (.cpu .bitand CPU_ISET_65C02)
        adc     #Offset                 ; Assume carry to be clear
.endif
        tax

        ; Check if the handshake setting is valid
        ldy     #SER_PARAMS::HANDSHAKE  ; Handshake
        lda     (ptr1),y
        cmp     #SER_HS_HW              ; This is all we support
        bne     InvParam

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

        ; Set the value for the control register, which contains stop bits,
        ; word length and the baud rate.
        ldy     #SER_PARAMS::BAUDRATE
        lda     (ptr1),y                ; Baudrate index
        tay
        lda     BaudTable,y             ; Get 6551 value
        bmi     InvBaud                 ; Branch if rate not supported
        sta     tmp1

        ldy     #SER_PARAMS::DATABITS   ; Databits
        lda     (ptr1),y
        tay
        lda     BitTable,y
        ora     tmp1
        sta     tmp1

        ldy     #SER_PARAMS::STOPBITS   ; Stopbits
        lda     (ptr1),y
        tay
        lda     StopTable,y
        ora     tmp1
        ora     #%00010000              ; Receiver clock source = baudrate
        sta     ACIA_CTRL,x

        ; Set the value for the command register. We remember the base value
        ; in RtsOff, since we will have to manipulate ACIA_CMD often.
        ldy     #SER_PARAMS::PARITY     ; Parity
        lda     (ptr1),y
        tay
        lda     ParityTable,y
        ora     #%00000001              ; DTR active
        sta     RtsOff
        ora     #%00001000              ; Enable receive interrupts
        sta     ACIA_CMD,x

        ; Done
        stx     Index                   ; Mark port as open
        lda     #SER_ERR_OK
        .assert SER_ERR_OK = 0, error
        tax
        rts

        ; Device (hardware) not found
NoDevice:lda    #SER_ERR_NO_DEVICE
        ldx     #0 ; return value is char
        rts

        ; Invalid parameter
InvParam:lda    #SER_ERR_INIT_FAILED
        ldx     #0 ; return value is char
        rts

        ; Baud rate not available
InvBaud:lda     #SER_ERR_BAUD_UNAVAIL
        ldx     #0 ; return value is char
        rts

;----------------------------------------------------------------------------
; SER_GET: Will fetch a character from the receive buffer and store it into the
; variable pointed to by ptr1. If no data is available, SER_ERR_NO_DATA is
; returned.

SER_GET:
        ldx     Index

        ; Check for buffer empty
        lda     RecvFreeCnt     ; (25)
        cmp     #$FF
        bne     :+
        lda     #SER_ERR_NO_DATA
        ldx     #0 ; return value is char
        rts

        ; Check for flow stopped & enough free: release flow control
:       ldy     Stopped         ; (34)
        beq     :+
        cmp     #63
        bcc     :+
        lda     #$00
        sta     Stopped
        lda     RtsOff
        ora     #%00001000
        sta     ACIA_CMD,x

        ; Get byte from buffer
:       ldy     RecvHead        ; (41)
        lda     RecvBuf,y
        inc     RecvHead
        inc     RecvFreeCnt
        ldx     #$00            ; (59)
.if (.cpu .bitand CPU_ISET_65C02)
        sta     (ptr1)
.else
        sta     (ptr1,x)
.endif
        txa                     ; Return code = 0
        rts

;----------------------------------------------------------------------------
; SER_PUT: Output character in A.
; Must return an SER_ERR_xx code in a/x.

SER_PUT:
        ldx     Index

        ; Try to send
        ldy     SendFreeCnt
        cpy     #$FF            ; Nothing to flush
        beq     :+
        pha
        lda     #$00            ; TryHard = false
        jsr     TryToSend
        pla

        ; Reload SendFreeCnt after TryToSend
        ldy     SendFreeCnt
        bne     :+
        lda     #SER_ERR_OVERFLOW
        ldx     #0 ; return value is char
        rts

        ; Put byte into send buffer & send
:       ldy     SendTail
        sta     SendBuf,y
        inc     SendTail
        dec     SendFreeCnt
        lda     #$FF            ; TryHard = true
        jsr     TryToSend
        lda     #SER_ERR_OK
        .assert SER_ERR_OK = 0, error
        tax
        rts

;----------------------------------------------------------------------------
; SER_STATUS: Return the status in the variable pointed to by ptr1.
; Must return an SER_ERR_xx code in a/x.

SER_STATUS:
        ldx     Index
        lda     ACIA_STATUS,x
        ldx     #$00
        sta     (ptr1,x)
        .assert SER_ERR_OK = 0, error
        txa
        rts

;----------------------------------------------------------------------------
; SER_IOCTL: Driver defined entry point. The wrapper will pass a pointer to ioctl
; specific data in ptr1, and the ioctl code in A.
; Must return an SER_ERR_xx code in a/x.

SER_IOCTL:
        ; Check data msb and code to be 0
        ora     ptr1+1
        bne     :+

        ; Check data lsb to be [1..7]
        ldx     ptr1
        beq     :+
        cpx     #7+1
        bcs     :+

        stx     Slot
        .assert SER_ERR_OK = 0, error
        tax
        rts

:       lda     #SER_ERR_INV_IOCTL
        ldx     #0 ; return value is char
        rts

;----------------------------------------------------------------------------
; SER_IRQ: Called from the builtin runtime IRQ handler as a subroutine. All
; registers are already saved, no parameters are passed, but the carry flag
; is clear on entry. The routine must return with carry set if the interrupt
; was handled, otherwise with carry clear.

SER_IRQ:
        ldx     Index           ; Check for open port
        beq     Done
        lda     ACIA_STATUS,x   ; Check ACIA status for receive interrupt
        and     #$08
        beq     Done            ; Jump if no ACIA interrupt
        lda     ACIA_DATA,x     ; Get byte from ACIA
        ldx     RecvFreeCnt     ; Check if we have free space left
        beq     Flow            ; Jump if no space in receive buffer
        ldy     RecvTail        ; Load buffer pointer
        sta     RecvBuf,y       ; Store received byte in buffer
        inc     RecvTail        ; Increment buffer pointer
        dec     RecvFreeCnt     ; Decrement free space counter
        cpx     #33             ; Check for buffer space low
        bcc     Flow            ; Assert flow control if buffer space low
        rts                     ; Interrupt handled (carry already set)

        ; Assert flow control if buffer space too low
Flow:   ldx     Index
lda     RtsOff
        sta     ACIA_CMD,x
        sta     Stopped
        sec                     ; Interrupt handled
Done:   rts

;----------------------------------------------------------------------------
; Try to send a byte. Internal routine. A = TryHard

TryToSend:
        sta     tmp1            ; Remember tryHard flag
NextByte:
        lda     SendFreeCnt
        cmp     #$FF
        beq     Quit            ; Bail out

        ; Check for flow stopped
Again:  lda     Stopped
        bne     Quit            ; Bail out

        ; Check that ACIA is ready to send
        lda     ACIA_STATUS,x
        and     #$10
        bne     Send
        bit     tmp1            ; Keep trying if must try hard
        bmi     Again
Quit:   rts

        ; Send byte and try again
Send:   ldy     SendHead
        lda     SendBuf,y
        sta     ACIA_DATA,x
        inc     SendHead
        inc     SendFreeCnt
        jmp     NextByte
