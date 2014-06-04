;
; Christian Groessler, Dec-2001
; converted to driver interface Dec-2013
;
; RS232 routines using the R: device (currently tested with an 850 only)
;

        .include        "zeropage.inc"
        .include        "ser-kernel.inc"
        .include        "ser-error.inc"
        .include        "atari.inc"

        .macpack        module


; ------------------------------------------------------------------------
; Header. Includes jump table

.ifdef __ATARIXL__
        module_header   _atrxrdev_ser
.else
        module_header   _atrrdev_ser
.endif

; Driver signature

        .byte   $73, $65, $72           ; "ser"
        .byte   SER_API_VERSION         ; Serial API version number

; Library reference

libref: .addr   $0000

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


        .rodata

rdev:   .byte   "R:", ATEOL, 0
bauds:  .byte   1               ; SER_BAUD_45_5
        .byte   2               ; SER_BAUD_50
        .byte   4               ; SER_BAUD_75
        .byte   5               ; SER_BAUD_110
        .byte   6               ; SER_BAUD_134_5
        .byte   7               ; SER_BAUD_150
        .byte   8               ; SER_BAUD_300
        .byte   9               ; SER_BAUD_600
        .byte   10              ; SER_BAUD_1200
        .byte   11              ; SER_BAUD_1800
        .byte   12              ; SER_BAUD_2400
        .byte   0               ; SER_BAUD_3600
        .byte   13              ; SER_BAUD_4800
        .byte   0               ; SER_BAUD_7200
        .byte   14              ; SER_BAUD_9600
        .byte   0               ; SER_BAUD_19200
        .byte   0               ; SER_BAUD_38400
        .byte   0               ; SER_BAUD_57600
        .byte   0               ; SER_BAUD_115200
        .byte   0               ; SER_BAUD_230400
        .byte   0               ; SER_BAUD_31250
        .byte   0               ; SER_BAUD_62500
        .byte   3               ; SER_BAUD_56_875
num_bauds       =       * - bauds
databits:
        .byte   48              ; SER_BITS_5
        .byte   32              ; SER_BITS_6
        .byte   16              ; SER_BITS_7
        .byte   0               ; SER_BITS_8
num_databits    =       * - databits
parities:
        .byte   0               ; SER_PAR_NONE
        .byte   4+1             ; SER_PAR_ODD
        .byte   2+8             ; SER_PAR_EVEN
        ;.byte  0               ; SER_PAR_MARK
        ;.byte  0               ; SER_PAR_SPACE
num_parities    =       * - parities

        .bss

; receive buffer
RECVBUF_SZ = 256
recv_buf: .res  RECVBUF_SZ

cm_run: .res    1       ; concurrent mode running?

        .data

rshand: .word   $ffff

; jump table into main program, initialized from libref
my_newfd:
        .byte   $4C
        .word   0
my__close:
        .byte   $4C
        .word   0
my_pushax:
        .byte   $4C
        .word   0
my_popax:
        .byte   $4C
        .word   0
my_findfreeiocb:
        .byte   $4C
        .word   0
my___do_oserror:
        .byte   $4C
        .word   0
my_fddecusage:
        .byte   $4C
        .word   0
my_fdtoiocb:
        .byte   $4C
        .word   0
my___inviocb:
        .byte   $4C
        .word   0
my_clriocb:
        .byte   $4C
        .word   0
my_CIOV:
        .byte   $4C
        .word   0

        .code

invbaud:
        lda     #<SER_ERR_BAUD_UNAVAIL
        ldx     #>SER_ERR_BAUD_UNAVAIL
openerr:
        rts


;----------------------------------------------------------------------------
; SER_OPEN: A pointer to a ser_params structure is passed in ptr1.
; Must return an SER_ERR_xx code in a/x.

SER_OPEN:
        jsr     do_open
        bne     openerr

; set line parameters
        lda     rshand
        ldx     #0
        jsr     my_fdtoiocb     ; get iocb index into X
        bmi     openerr         ; shouldn't happen
        tax

        ; set baud rate, word size, stop bits and ready monitoring

        ; build ICAX1 value
        ldy     #SER_PARAMS::BAUDRATE
        lda     (ptr1),y
        cmp     #num_bauds
        bcs     invbaud

        tay
        lda     bauds,y
        beq     invbaud
        sta     ICAX1,x

        ldy     #SER_PARAMS::DATABITS
        lda     (ptr1),y
        cmp     #num_databits
        bcs     init_err

        tay
        lda     databits,y
        ora     ICAX1,x
        sta     ICAX1,x

        ldy     #SER_PARAMS::STOPBITS
        lda     (ptr1),y
        clc
        ror     a
        ror     a
        ora     ICAX1,x
        sta     ICAX1,x

        lda     #36             ; xio 36, baud rate
        sta     ICCOM,x
        lda     #0
        ;ICAX2 = 0, monitor nothing
        sta     ICAX2,x
        sta     ICBLL,x
        sta     ICBLH,x
        sta     ICBAL,x
        sta     ICBAH,x
        jsr     my_CIOV
        bmi     cioerr

        ; check if the handshake setting is valid
        ldy     #SER_PARAMS::HANDSHAKE
        lda     (ptr1),y
        cmp     #SER_HS_HW      ; this is all we support
        bne     init_err

        ; set handshake lines
        lda     #34             ; xio 34, set cts, dtr etc
        sta     ICCOM,x
        lda     #192+48+3       ; DTR on, RTS on, XMT on
        sta     ICAX1,x
        jsr     my_CIOV
        bmi     cioerr

        ; set translation and parity
        ldy     #SER_PARAMS::PARITY
        lda     (ptr1),y
        cmp     #num_parities
        bcs     init_err

        tay
        lda     parities,y
        ora     #32             ; no translation
        sta     ICAX1,x

        lda     #38             ; xio 38, translation and parity
        sta     ICCOM,x
        jsr     my_CIOV
        bmi     cioerr

        lda     #<SER_ERR_OK
        tax                             ; A is zero
        rts

inverr: jmp     my___inviocb

cioerr:
        ; @@@ need to close IOCB here
        jsr     my_fddecusage   ; decrement usage counter of fd as open failed

init_err:
        ldx     #0
        lda     #SER_ERR_INIT_FAILED
        rts

;---- open the device

do_open:
        jsr     my_findfreeiocb
        bne     init_err
        txa
        tay                     ; move iocb # into Y
        lda     #3
        sta     tmp3            ; name length + 1
        lda     #<rdev
        ldx     #>rdev
        jsr     my_newfd
        tya
        bcs     @doopen         ; C set: open needed / device not already open

        pha
        jsr     SER_CLOSE       ;** shut down if started  @@@TODO check this out!!
        pla

@doopen:tax
        pha
        jsr     my_clriocb
        pla
        tax
        lda     #<rdev
        sta     ICBAL,x
        lda     #>rdev
        sta     ICBAH,x
        lda     #OPEN
        sta     ICCOM,x

        lda     #$0D            ; mode in+out+concurrent
        sta     ICAX1,x
        lda     #0
        sta     ICAX2,x
        sta     ICBLL,x         ; zap buf len
        sta     ICBLH,x
        jsr     my_CIOV
        bmi     cioerr

        lda     tmp2            ; get fd (from newfd)
        sta     rshand
        ldx     #0
        stx     rshand+1
        txa
        rts

;----------------------------------------------------------------------------
; CLOSE: Close the port, disable interrupts and flush the buffer. Called
; without parameters. Must return an error code in a/x.
;
;----------------------------------------------------------------------------
; SER_UNINSTALL routine. Is called before the driver is removed from memory.
; Must return an SER_ERR_xx code in a/x.
;

SER_UNINSTALL:
SER_CLOSE:
        lda     rshand
        cmp     #$ff
        beq     @done

        ldx     rshand+1
        jsr     my__close
        ldx     #$ff
        stx     rshand
        stx     rshand+1
        inx
        stx     cm_run
@done:  lda     #<SER_ERR_OK
        ldx     #>SER_ERR_OK
        rts

;----------------------------------------------------------------------------
; SER_GET: Will fetch a character from the receive buffer and store it into the
; variable pointer to by ptr1. If no data is available, SER_ERR_NO_DATA is
; return.
;

SER_GET:
        ldy     rshand
        cpy     #$ff
        beq     ni_err           ; work only if initialized

        lda     rshand
        ldx     #0
        jsr     my_fdtoiocb
        tax
        lda     cm_run          ; concurrent mode already running?
        bne     @go
        jsr     ena_cm          ; turn on concurrent mode

@go:    ; check whether there is any input available

        lda     #STATIS         ; status request, returns bytes pending
        sta     ICCOM,x
        jsr     my_CIOV
        bmi     ser_error

        lda     DVSTAT+1        ; get byte count pending
        ora     DVSTAT+2
        beq     @nix_da         ; no input waiting...

        ; input is available: get it!

        lda     #GETCHR         ; get raw bytes
        sta     ICCOM,x         ; in command code
        lda     #0
        sta     ICBLL,x
        sta     ICBLH,x
        sta     ICBAL,x
        sta     ICBAH,x
        jsr     my_CIOV         ; go get it
        bmi     ser_error

        ldx     #0
        sta     (ptr1,x)        ; return received byte
        txa
        rts

@nix_da:lda     #SER_ERR_NO_DATA
        ldx     #0
        rts

ser_error:
        lda     #SER_ERR_OVERFLOW       ; there is no large selection of serial error codes... :-/
        ldx     #0
        rts

ni_err: lda     #SER_ERR_NOT_OPEN
        ldx     #0
        rts

;----------------------------------------------------------------------------
; SER_PUT: Output character in A.
; Must return an error code in a/x.
;

SER_PUT:
        ldy     rshand
        cpy     #$ff
        beq     ni_err          ; work only if initialized

        pha                     ; remember char to write
        lda     rshand
        ldx     #0
        jsr     my_fdtoiocb
        tax

        lda     cm_run          ; concurrent mode already running?
        bne     @go
        jsr     ena_cm          ; turn on concurrent mode

        ; @@@TODO:       check output buffer overflow
@go:    lda     #PUTCHR         ; put raw bytes
        sta     ICCOM,x         ; in command code
        lda     #0
        sta     ICBLL,x
        sta     ICBLH,x
        sta     ICBAL,x
        sta     ICBAH,x
        pla                     ; get the char back
        jsr     my_CIOV         ; go do it
        bmi     ser_error
        lda     #0
        tax
        rts

;----------------------------------------------------------------------------
; SER_STATUS: Return the status in the variable pointed to by ptr1.
; Must return an error code in a/x.
;

SER_STATUS:
        ; fall through to SER_IOCTL

;----------------------------------------------------------------------------
; SER_IOCTL: Driver defined entry point. The wrapper will pass a pointer to ioctl
; specific data in ptr1, and the ioctl code in A.
; Must return an error code in a/x.
;

SER_IOCTL:
        lda     #<SER_ERR_INV_IOCTL     ; We don't support ioclts for now
        ldx     #>SER_ERR_INV_IOCTL
        rts

;----------------------------------------------------------------------------
; SER_IRQ: Not used on the Atari
;

SER_IRQ     = $0000

;----------------------------------------------------------------------------
; SER_INSTALL routine. Is called after the driver is loaded into memory. If
; possible, check if the hardware is present.
; Must return an SER_ERR_xx code in a/x.

SER_INSTALL:
        ; check if R: device is installed
        ldy     #0
search: lda     HATABS,y                ; get device name
        cmp     #'R'
        beq     found
        iny
        iny
        iny
        cpy     #MAXDEV
        bcc     search

; R: device not found, return error

        lda     #<SER_ERR_NO_DEVICE
        ldx     #0
        rts

; R: device found, initialize jump table into main program

found:  lda     ptr3
        pha
        lda     ptr3+1
        pha
        lda     libref
        sta     ptr3
        lda     libref+1
        sta     ptr3+1

        ldy     #0
        lda     (ptr3),y
        sta     my_newfd+1
        iny
        lda     (ptr3),y
        sta     my_newfd+2
        iny

        lda     (ptr3),y
        sta     my__close+1
        iny
        lda     (ptr3),y
        sta     my__close+2
        iny

        lda     (ptr3),y
        sta     my_pushax+1
        iny
        lda     (ptr3),y
        sta     my_pushax+2
        iny

        lda     (ptr3),y
        sta     my_popax+1
        iny
        lda     (ptr3),y
        sta     my_popax+2
        iny

        lda     (ptr3),y
        sta     my_findfreeiocb+1
        iny
        lda     (ptr3),y
        sta     my_findfreeiocb+2
        iny

        lda     (ptr3),y
        sta     my___do_oserror+1
        iny
        lda     (ptr3),y
        sta     my___do_oserror+2
        iny

        lda     (ptr3),y
        sta     my_fddecusage+1
        iny
        lda     (ptr3),y
        sta     my_fddecusage+2
        iny

        lda     (ptr3),y
        sta     my_fdtoiocb+1
        iny
        lda     (ptr3),y
        sta     my_fdtoiocb+2
        iny

        lda     (ptr3),y
        sta     my___inviocb+1
        iny
        lda     (ptr3),y
        sta     my___inviocb+2
        iny

        lda     (ptr3),y
        sta     my_clriocb+1
        iny
        lda     (ptr3),y
        sta     my_clriocb+2
        iny

        lda     (ptr3),y
        sta     my_CIOV+1
        iny
        lda     (ptr3),y
        sta     my_CIOV+2
        ;iny

        pla
        sta     ptr3+1
        pla
        sta     ptr3

        lda     #<SER_ERR_OK
        tax                     ; A is zero
        rts


; enable concurrent rs232 mode
; gets iocb index in X
; all registers destroyed

.proc   ena_cm

        lda     #40             ; XIO 40, start concurrent IO
        sta     ICCOM,x
        sta     cm_run          ; indicate concurrent mode is running
        lda     #$0D            ; value from 850 manual, p62.  must be $0D?,
        sta     ICAX1,x         ;  or any non-zero?
        lda     #0
        sta     ICAX2,x
        lda     #<recv_buf
        sta     ICBAL,x
        lda     #>recv_buf
        sta     ICBAH,x
        lda     #<RECVBUF_SZ
        sta     ICBLL,x
        lda     #>RECVBUF_SZ
        sta     ICBLH,x
        jmp     my_CIOV

.endproc        ;ena_cm
