;
; Serial driver for the builtin 6551 ACIA of the Commodore 510.
;
; Ullrich von Bassewitz, 2003-12-18
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
        .include        "../extzp.inc"
        .include        "ser-kernel.inc"
        .include        "ser-error.inc"
        .include        "cbm510.inc"

        .macpack        module


; ------------------------------------------------------------------------
; Header. Includes jump table

        module_header   _cbm510_std_ser

; Driver signature

        .byte   $73, $65, $72           ; "ser"
        .byte   SER_API_VERSION         ; Serial API version number

; Library reference

        .addr   $0000

; Jump table

        .word   SER_INSTALL
        .word   SER_UNINSTALL
        .word   SER_OPEN
        .word   SER_CLOSE
        .word   SER_GET
        .word   SER_PUT
        .word   SER_STATUS
        .word   SER_IOCTL
        .word   SER_IRQ

;----------------------------------------------------------------------------
;
; Global variables
;

.bss
RecvHead:       .res    1       ; Head of receive buffer
RecvTail:       .res    1       ; Tail of receive buffer
RecvFreeCnt:    .res    1       ; Number of bytes in receive buffer
SendHead:       .res    1       ; Head of send buffer
SendTail:       .res    1       ; Tail of send buffer
SendFreeCnt:    .res    1       ; Number of bytes in send buffer

Stopped:        .res    1       ; Flow-stopped flag
RtsOff:         .res    1       ;

; Send and receive buffers: 256 bytes each
RecvBuf:        .res    256
SendBuf:        .res    256

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

.code

;----------------------------------------------------------------------------
; SER_INSTALL routine. Is called after the driver is loaded into memory. If
; possible, check if the hardware is present.
; Must return an SER_ERR_xx code in a/x.
;
; Since we don't have to manage the IRQ vector on the Plus/4, this is actually
; the same as:
;
; SER_UNINSTALL routine. Is called before the driver is removed from memory.
; Must return an SER_ERR_xx code in a/x.
; and:
;
; SER_CLOSE: Close the port, disable interrupts and flush the buffer. Called
; without parameters. Must return an error code in a/x.
;

SER_INSTALL:
SER_UNINSTALL:
SER_CLOSE:

; Deactivate DTR and disable 6551 interrupts

        lda     #%00001010
        jsr     write_cmd

; Done, return an error code

        lda     #SER_ERR_OK
        .assert SER_ERR_OK = 0, error
        tax
        rts

;----------------------------------------------------------------------------
; PARAMS routine. A pointer to a ser_params structure is passed in ptr1.
; Must return an SER_ERR_xx code in a/x.

SER_OPEN:

; Check if the handshake setting is valid

        ldy     #SER_PARAMS::HANDSHAKE  ; Handshake
        lda     (ptr1),y
        cmp     #SER_HS_HW              ; This is all we support
        bne     InvParam

; Initialize buffers

        ldx     #0
        stx     Stopped
        stx     RecvHead
        stx     RecvTail
        stx     SendHead
        stx     SendTail
        dex                             ; X = 255
        stx     RecvFreeCnt
        stx     SendFreeCnt

; Set the value for the control register, which contains stop bits, word
; length and the baud rate.

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
        ldy     #ACIA::CTRL
        jsr     write

; Set the value for the command register. We remember the base value in
; RtsOff, since we will have to manipulate ACIA_CMD often.

        ldy     #SER_PARAMS::PARITY     ; Parity
        lda     (ptr1),y
        tay
        lda     ParityTable,y
        ora     #%00000001              ; DTR active
        sta     RtsOff
        ora     #%00001000              ; Enable receive interrupts
        jsr     write_cmd

; Done

        lda     #SER_ERR_OK
        .assert SER_ERR_OK = 0, error
        tax
        rts

; Invalid parameter

InvParam:
        lda     #SER_ERR_INIT_FAILED
        ldx     #0 ; return value is char
        rts

; Baud rate not available

InvBaud:
        lda     #SER_ERR_BAUD_UNAVAIL
        ldx     #0 ; return value is char
        rts

;----------------------------------------------------------------------------
; SER_GET: Will fetch a character from the receive buffer and store it into the
; variable pointer to by ptr1. If no data is available, SER_ERR_NO_DATA is
; return.
;

SER_GET:
        ldx     SendFreeCnt             ; Send data if necessary
        inx                             ; X == $FF?
        beq     @L1
        lda     #$00
        jsr     TryToSend

; Check for buffer empty

@L1:    lda     RecvFreeCnt
        cmp     #$ff
        bne     @L2
        lda     #SER_ERR_NO_DATA
        ldx     #0 ; return value is char
        rts

; Check for flow stopped & enough free: release flow control

@L2:    ldx     Stopped
        beq     @L3
        cmp     #63
        bcc     @L3
        lda     #$00
        sta     Stopped
        lda     RtsOff
        ora     #%00001000
        jsr     write_cmd

; Get byte from buffer

@L3:    ldx     RecvHead
        lda     RecvBuf,x
        inc     RecvHead
        inc     RecvFreeCnt
        ldx     #$00
        sta     (ptr1,x)
        txa                             ; Return code = 0
        rts

;----------------------------------------------------------------------------
; SER_PUT: Output character in A.
; Must return an error code in a/x.
;

SER_PUT:

; Try to send

        ldx     SendFreeCnt
        inx                             ; X = $ff?
        beq     @L2
        pha
        lda     #$00
        jsr     TryToSend
        pla

; Put byte into send buffer & send

@L2:    ldx     SendFreeCnt
        bne     @L3
        lda     #SER_ERR_OVERFLOW      ; X is already zero
        rts

@L3:    ldx     SendTail
        sta     SendBuf,x
        inc     SendTail
        dec     SendFreeCnt
        lda     #$ff
        jsr     TryToSend
        lda     #SER_ERR_OK
        .assert SER_ERR_OK = 0, error
        tax
        rts

;----------------------------------------------------------------------------
; SER_STATUS: Return the status in the variable pointed to by ptr1.
; Must return an error code in a/x.
;

SER_STATUS:
        lda     #$0F
        sta     IndReg
        ldy     #ACIA::STATUS
        lda     (acia),y
        ldx     #0
        sta     (ptr1,x)
        lda     IndReg
        sta     ExecReg
        .assert SER_ERR_OK = 0, error
        txa
        rts

;----------------------------------------------------------------------------
; SER_IOCTL: Driver defined entry point. The wrapper will pass a pointer to ioctl
; specific data in ptr1, and the ioctl code in A.
; Must return an error code in a/x.
;

SER_IOCTL:
        lda     #SER_ERR_INV_IOCTL      ; We don't support ioclts for now
        ldx     #0 ; return value is char
        rts

;----------------------------------------------------------------------------
; SER_IRQ: Called from the builtin runtime IRQ handler as a subroutine. All
; registers are already save, no parameters are passed, but the carry flag
; is clear on entry. The routine must return with carry set if the interrupt
; was handled, otherwise with carry clear.
;

SER_IRQ:
        lda     #$0F
        sta     IndReg          ; Switch to the system bank
        ldy     #ACIA::STATUS
        lda     (acia),y        ; Check ACIA status for receive interrupt
        and     #$08
        beq     @L9             ; Jump if no ACIA interrupt (carry still clear)
        ldy     #ACIA::DATA
        lda     (acia),y        ; Get byte from ACIA
        ldx     RecvFreeCnt     ; Check if we have free space left
        beq     @L1             ; Jump if no space in receive buffer
        ldy     RecvTail        ; Load buffer pointer
        sta     RecvBuf,y       ; Store received byte in buffer
        inc     RecvTail        ; Increment buffer pointer
        dec     RecvFreeCnt     ; Decrement free space counter
        cpx     #33             ; Check for buffer space low
        bcs     @L9             ; Assert flow control if buffer space low

; Assert flow control if buffer space too low

@L1:    lda     RtsOff
        ldy     #ACIA::CMD
        sta     (acia),y
        sta     Stopped
        sec                     ; Interrupt handled

; Done, switch back to the execution segment

@L9:    lda     ExecReg
        sta     IndReg
        rts

;----------------------------------------------------------------------------
; Try to send a byte. Internal routine. A = TryHard

.proc   TryToSend

        sta     tmp1            ; Remember tryHard flag
        lda     #$0F
        sta     IndReg          ; Switch to the system bank
@L0:    lda     SendFreeCnt
        cmp     #$ff
        beq     @L3             ; Bail out

; Check for flow stopped

@L1:    lda     Stopped
        bne     @L3             ; Bail out

; Check that swiftlink is ready to send

@L2:    ldy     #ACIA::STATUS
        lda     (acia),y
        and     #$10
        bne     @L4
        bit     tmp1            ; Keep trying if must try hard
        bmi     @L0

; Switch back the bank and return

@L3:    lda     ExecReg
        sta     IndReg
        rts

; Send byte and try again

@L4:    ldx     SendHead
        lda     SendBuf,x
        ldy     #ACIA::DATA
        sta     (acia),y
        inc     SendHead
        inc     SendFreeCnt
        jmp     @L0

.endproc


;----------------------------------------------------------------------------
; Write to the ACIA changing the indirect segment. Offset is in Y, value in A.

write_cmd:
        ldy     #ACIA::CMD
write:  pha
        lda     #$0F
        sta     IndReg
        pla
        sta     (acia),y
        lda     ExecReg
        sta     IndReg
        rts
