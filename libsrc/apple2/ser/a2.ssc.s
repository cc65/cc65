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
        .byte   $73, $65, $72   ; "ser"
        .byte   SER_API_VERSION ; Serial API version number

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
ACIA           := $C088
.else
Offset          = $8F           ; Move 6502 false read out of I/O to page $BF
ACIA           := $C088-Offset
.endif

ACIA_DATA      := ACIA+0        ; Data register
ACIA_STATUS    := ACIA+1        ; Status register
ACIA_CMD       := ACIA+2        ; Command register
ACIA_CTRL      := ACIA+3        ; Control register

SLTROMSEL      := $C02D         ; For Apple IIgs slot verification

;----------------------------------------------------------------------------
; Global variables

        .bss

RecvHead:       .res    1       ; Head of receive buffer
RecvTail:       .res    1       ; Tail of receive buffer
RecvFreeCnt:    .res    1       ; Number of free bytes in receive buffer
SendHead:       .res    1       ; Head of send buffer
SendTail:       .res    1       ; Tail of send buffer
SendFreeCnt:    .res    1       ; Number of free bytes in send buffer

Stopped:        .res    1       ; Flow-stopped flag
RtsOff:         .res    1       ; Cached value of command register with
                                ; flow stopped
HSType:         .res    1       ; Flow-control type

RecvBuf:        .res    256     ; Receive buffer: 256 bytes
SendBuf:        .res    256     ; Send buffer: 256 bytes

Index:          .res    1       ; I/O register index

        .data

Slot:   .byte   $02             ; Default to SSC in slot 2

        .rodata

BaudTable:                      ; Table used to translate RS232 baudrate param
                                ; into control register value
                                ; bit7 = 1 means setting is invalid
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
        .byte   $00             ; SER_BAUD_115200
        .byte   $FF             ; SER_BAUD_230400

BitTable:                       ; Table used to translate RS232 databits param
                                ; into control register value
        .byte   $60             ; SER_BITS_5
        .byte   $40             ; SER_BITS_6
        .byte   $20             ; SER_BITS_7
        .byte   $00             ; SER_BITS_8

StopTable:                      ; Table used to translate RS232 stopbits param
                                ; into control register value
        .byte   $00             ; SER_STOP_1
        .byte   $80             ; SER_STOP_2

ParityTable:                    ; Table used to translate RS232 parity param
                                ; into command register value
        .byte   $00             ; SER_PAR_NONE
        .byte   $20             ; SER_PAR_ODD
        .byte   $60             ; SER_PAR_EVEN
        .byte   $A0             ; SER_PAR_MARK
        .byte   $E0             ; SER_PAR_SPACE

IdOfsTable:                     ; Table of bytes positions, used to check four
                                ; specific bytes on the slot's firmware to make
                                ; sure this is a serial card.
        .byte   $05             ; Pascal 1.0 ID byte
        .byte   $07             ; Pascal 1.0 ID byte
        .byte   $0B             ; Pascal 1.1 generic signature byte
        .byte   $0C             ; Device signature byte

IdValTable:                     ; Table of expected values for the four checked
                                ; bytes
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

        lda     #%00001010      ; Deactivate DTR and disable 6551 interrupts
        sta     ACIA_CMD,x

:       lda     #SER_ERR_OK     ; Done, return an error code
        .assert SER_ERR_OK = 0, error
        tax
        stx     Index           ; Mark port as closed
        rts

;----------------------------------------------------------------------------
; SER_OPEN: A pointer to a ser_params structure is passed in ptr1.
; Must return an SER_ERR_xx code in a/x.
; Note: Hardware checks are done in SER_OPEN instead of SER_INSTALL,
; because they depend on the selected slot, and we can't select the slot
; before SER_INSTALL.

SER_OPEN:
        ; Check if this is a IIgs (Apple II Miscellaneous TechNote #7,
        ; Apple II Family Identification)
        sec
        bit     $C082
        jsr     $FE1F
        bit     $C080

        bcs     NotIIgs

        ; We're on a IIgs. For every slot N, either bit N of $C02D is
        ; 0 for the internal ROM, or 1 for "Your Card". Let's make sure
        ; that slot N's bit is set to 1, otherwise, that can't be an SSC.

        ldy     Slot
        lda     SLTROMSEL
:       lsr
        dey
        bpl     :-              ; Shift until slot's bit ends in carry
        bcc     NoDev

NotIIgs:ldx     #<$C000
        stx     ptr2
        lda     #>$C000
        ora     Slot
        sta     ptr2+1

:       ldy     IdOfsTable,x    ; Check Pascal 1.1 Firmware Protocol ID bytes
        lda     IdValTable,x
        cmp     (ptr2),y
        bne     NoDev
        inx
        cpx     #IdTableLen
        bcc     :-

        lda     Slot            ; Convert slot to I/O register index
        asl
        asl
        asl
        asl
.if .not (.cpu .bitand CPU_ISET_65C02)
        adc     #Offset         ; Assume carry to be clear
.endif
        tax

        ; Check that this works like an ACIA 6551 is expected to work

        lda     ACIA_STATUS,x   ; Save current values in what we expect to be
        sta     tmp1            ; the ACIA status register
        lda     ACIA_CMD,x      ; and command register. So we can restore them
        sta     tmp2            ; if this isn't a 6551.

        ldy     #%00000010      ; Disable TX/RX, disable IRQ
:       tya
        sta     ACIA_CMD,x
        cmp     ACIA_CMD,x      ; Verify what we stored is there
        bne     NotAcia
        iny                     ; Enable TX/RX, disable IRQ
        cpy     #%00000100
        bne     :-
        sta     ACIA_STATUS,x   ; Reset ACIA
        lda     ACIA_CMD,x      ; Check that RX/TX is disabled
        lsr
        bcc     AciaOK

NotAcia:lda     tmp2            ; Restore original values
        sta     ACIA_CMD,x
        lda     tmp1
        sta     ACIA_STATUS,x

NoDev:  lda     #SER_ERR_NO_DEVICE
        bne     Out

        ; Check if the handshake setting is valid
AciaOK: ldy     #SER_PARAMS::HANDSHAKE
        lda     (ptr1),y
        cmp     #SER_HS_SW      ; Not supported
        bne     HandshakeOK

        lda     #SER_ERR_INIT_FAILED
        bne     Out

HandshakeOK:
        sta     HSType          ; Store flow control type

        ldy     #$00            ; Initialize buffers
        sty     Stopped
        sty     RecvHead
        sty     RecvTail
        sty     SendHead
        sty     SendTail
        dey                     ; Y = 255
        sty     RecvFreeCnt
        sty     SendFreeCnt

        ; Set the value for the control register, which contains stop bits,
        ; word length and the baud rate.
        ldy     #SER_PARAMS::BAUDRATE
        lda     (ptr1),y        ; Baudrate index
        tay
        lda     BaudTable,y     ; Get 6551 value
        sta     tmp2            ; Backup for IRQ setting
        bpl     BaudOK          ; Check that baudrate is supported

        lda     #SER_ERR_BAUD_UNAVAIL
        bne     Out

BaudOK: sta     tmp1
        ldy     #SER_PARAMS::DATABITS
        lda     (ptr1),y        ; Databits index
        tay
        lda     BitTable,y      ; Get 6551 value
        ora     tmp1
        sta     tmp1

        ldy     #SER_PARAMS::STOPBITS
        lda     (ptr1),y        ; Stopbits index
        tay
        lda     StopTable,y     ; Get 6551 value
        ora     tmp1
        ora     #%00010000      ; Set receiver clock source = baudrate
        sta     ACIA_CTRL,x

        ; Set the value for the command register. We remember the base value
        ; in RtsOff, since we will have to manipulate ACIA_CMD often.
        ldy     #SER_PARAMS::PARITY
        lda     (ptr1),y        ; Parity index
        tay
        lda     ParityTable,y   ; Get 6551 value

        ora     #%00000001      ; Set DTR active
        sta     RtsOff          ; Store value to easily handle flow control later

        ora     #%00001010      ; Disable interrupts and set RTS low

        ldy     tmp2            ; Don't enable IRQs if 115200bps
        beq     :+
        and     #%11111101      ; Enable receive IRQs
:       sta     ACIA_CMD,x

        ; Done
        stx     Index           ; Mark port as open
        lda     #SER_ERR_OK
Out:
        ldx     #$00            ; Promote char return value
        rts

;----------------------------------------------------------------------------
; SER_GET: Will fetch a character from the receive buffer and store it into the
; variable pointed to by ptr1. If no data is available, SER_ERR_NO_DATA is
; returned.

SER_GET:
        ldx     Index

        lda     RecvFreeCnt     ; Check for buffer empty
        cmp     #$FF
        bne     :+
        lda     #SER_ERR_NO_DATA
        ldx     #$00            ; Promote char return value
        rts

:       ldy     Stopped         ; Check for flow stopped
        beq     :+
        cmp     #63             ; Enough free?
        bcc     :+
.if (.cpu .bitand CPU_ISET_65C02)
        stz     Stopped         ; Release flow control
.else
        lda     #$00
        sta     Stopped
.endif
        lda     RtsOff
        ora     #%00001000
        sta     ACIA_CMD,x

:       ldy     RecvHead        ; Get byte from buffer
        lda     RecvBuf,y
        inc     RecvHead
        inc     RecvFreeCnt
        ldx     #$00
.if (.cpu .bitand CPU_ISET_65C02)
        sta     (ptr1)          ; Store it for caller
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

        ldy     SendFreeCnt     ; Anything to send first?
        cpy     #$FF            ; No
        beq     :+
        pha
        lda     #$00            ; TryHard = false
        jsr     TryToSend       ; Try to flush send buffer
        pla

        ldy     SendFreeCnt     ; Reload SendFreeCnt after TryToSend
        bne     :+
        lda     #SER_ERR_OVERFLOW
        ldx     #$00            ; Promote char return value
        rts

:       ldy     SendTail        ; Put byte into send buffer
        sta     SendBuf,y
        inc     SendTail
        dec     SendFreeCnt
        lda     #$FF            ; TryHard = true
        jsr     TryToSend       ; Flush send buffer
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
; The ioctl data is the slot number to open.
; Must return an SER_ERR_xx code in a/x.

SER_IOCTL:
        ora     ptr1+1          ; Check data msb and code to be 0
        bne     :+

        ldx     ptr1            ; Check data lsb to be [1..7]
        beq     :+
        cpx     #7+1
        bcs     :+

        stx     Slot            ; Store slot
        .assert SER_ERR_OK = 0, error
        tax
        rts

:       lda     #SER_ERR_INV_IOCTL
        ldx     #$00            ; Promote char return value
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

Flow:   lda     HSType          ; Don't touch if no flow control
        beq     IRQDone

        ldx     Index           ; Assert flow control if buffer space too low
        lda     RtsOff
        sta     ACIA_CMD,x
        sta     Stopped
IRQDone:sec                     ; Interrupt handled
Done:   rts

;----------------------------------------------------------------------------
; Try to send a byte. Internal routine. A = TryHard

TryToSend:
        sta     tmp1            ; Remember tryHard flag
NextByte:
        lda     SendFreeCnt     ; Is there anything to send? This can happen if
        cmp     #$FF            ; we got interrupted by RX while sending, and
        beq     Quit            ; flow control was asserted.

Again:  lda     Stopped         ; Is flow stopped?
        bne     Quit            ; Yes, Bail out

        lda     ACIA_STATUS,x   ; Check that ACIA is ready to send
        and     #$10
        bne     Send            ; It is!
        bit     tmp1            ; Keep trying if must try hard
        bmi     Again
Quit:   rts

Send:   ldy     SendHead        ; Get first byte to send
        lda     SendBuf,y
        sta     ACIA_DATA,x     ; Send it
        inc     SendHead
        inc     SendFreeCnt
        bne     NextByte        ; And try next one
