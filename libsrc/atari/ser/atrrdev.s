;
; Christian Groessler, Dec-2001
; converted to driver interface Nov-2013
;
; RS232 routines using the R: device (currently tested with an 850 only)
;

        .include        "zeropage.inc"
        .include        "ser-kernel.inc"
        .include        "ser-error.inc"
        .include        "atari.inc"

.macro	pushall
	php
	pha
	txa
	pha
	tya
	pha
.endmacro
.macro	pullall
	pla
	tay
	pla
	tax
	pla
	plp
.endmacro

.ifdef __ATARIXL__
.macro print_string text
.endmacro
.else
.macro print_string text
        .local  @start, @cont
        jmp     @cont
@start:  .byte   text, ATEOL
@cont:   php
	pha
	txa
	pha
	tya
	pha
	ldx     #0              ; channel 0
        lda     #<@start
        sta     ICBAL,x         ; address
        lda     #>@start
        sta     ICBAH,x
        lda     #<(@cont - @start)
        sta     ICBLL,x         ; length
        lda     #>(@cont - @start)
        sta     ICBLH,x
        lda     #PUTCHR
        sta     ICCOM,x
        jsr     CIOV
	pla
	tay
	pla
	tax
	pla
	plp
.endmacro
.endif

; ------------------------------------------------------------------------
; Header. Includes jump table

.segment        "JUMPTABLE"

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
bauds:	.byte	1		; SER_BAUD_45_5
	.byte	2		; SER_BAUD_50
	.byte	4		; SER_BAUD_75
	.byte	5		; SER_BAUD_110
	.byte	6		; SER_BAUD_134_5
	.byte	7		; SER_BAUD_150
	.byte	8		; SER_BAUD_300
	.byte	9		; SER_BAUD_600
	.byte	10		; SER_BAUD_1200
	.byte	11		; SER_BAUD_1800
	.byte	12		; SER_BAUD_2400
	.byte	0		; SER_BAUD_3600
	.byte	13		; SER_BAUD_4800
	.byte	0		; SER_BAUD_7200
	.byte	14		; SER_BAUD_9600
	.byte	0		; SER_BAUD_19200
	.byte	0		; SER_BAUD_38400
	.byte	0		; SER_BAUD_57600
	.byte	0		; SER_BAUD_115200
	.byte	0		; SER_BAUD_230400
	.byte	0		; SER_BAUD_31250
	.byte	0		; SER_BAUD_62500
	.byte	3		; SER_BAUD_56_875
num_bauds	=	* - bauds
databits:
	.byte	48		; SER_BITS_5
	.byte	32		; SER_BITS_6
	.byte	16		; SER_BITS_7
	.byte	0		; SER_BITS_8
num_databits	=	* - databits
parities:
	.byte	0		; SER_PAR_NONE
	.byte	4+1		; SER_PAR_ODD
	.byte	2+8		; SER_PAR_EVEN
	;.byte	0		; SER_PAR_MARK
	;.byte	0		; SER_PAR_SPACE
num_parities	=	* - parities

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
	jsr	do_open
	bne	openerr

; set line parameters
        lda     rshand
        ldx     #0
        jsr     my_fdtoiocb     ; get iocb index into X
        bmi     openerr         ; shouldn't happen
        tax

        ; set baud rate, word size, stop bits and ready monitoring

	; build ICAX1 value

	jsr	dump
	jsr	print_open_txt
	jsr	print_iocb_txt
	jsr	dump_iocb_num
	jsr	nl

	ldy     #SER_PARAMS::BAUDRATE
	lda	(ptr1),y
	cmp	#num_bauds
	bcs	invbaud

	tay
	lda	bauds,y
	beq	invbaud
	sta	ICAX1,x

	ldy     #SER_PARAMS::DATABITS
	lda	(ptr1),y
	cmp	#num_databits
	bcs	init_err

	tay
	lda	databits,y
	ora	ICAX1,x
	sta	ICAX1,x

	ldy     #SER_PARAMS::STOPBITS
	lda	(ptr1),y
	clc
	ror	a
	ror	a
	ora	ICAX1,x
	sta	ICAX1,x

	jsr	dump_aux1

        lda     #36             ; xio 36, baud rate
        sta     ICCOM,x
	lda	#0
        ;ICAX2 = 0, monitor nothing
	sta	ICAX2,x
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
	cmp	#num_parities
	bcs	init_err

	tay
	lda	parities,y
	ora	#32		; no translation
        sta     ICAX1,x

	jsr	dump_aux1

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

SER_CLOSE:
	pushall
	print_string "SER_CLOSE called"
	pullall
        lda     rshand
        cmp     #$ff
        beq     @done

	pushall
	print_string "SER_CLOSE do work"
	pullall

        ldx     rshand+1
        ;jsr     my_pushax
        jsr     my__close
        ldx     #$ff
        stx     rshand
        stx     rshand+1
        inx
        stx     cm_run
@done:  lda     #<SER_ERR_OK
        ldx     #>SER_ERR_OK
	pushall
	print_string "SER_CLOSE returns"
	pullall
        rts

;----------------------------------------------------------------------------
; SER_GET: Will fetch a character from the receive buffer and store it into the
; variable pointer to by ptr1. If no data is available, SER_ERR_NO_DATA is
; return.
;

SER_GET:
        ldy     rshand
        cpy     #$ff
        bne     @work           ; work only if initialized
        lda     #SER_ERR_NOT_OPEN
        bne     nierr

@work:  lda     rshand
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
	lda     #SER_ERR_OVERFLOW	; there is no large selection of serial error codes... :-/
        ldx     #0
        rts

nierr:  ldx     #0
        rts

;----------------------------------------------------------------------------
; SER_PUT: Output character in A.
; Must return an error code in a/x.
;

SER_PUT:
        ldy     rshand
        cpy     #$ff
        bne     @work           ; work only if initialized
        lda     #SER_ERR_NOT_OPEN
        bne     nierr

@work:  pha			; char to write
        lda     rshand
        ldx     #0
        jsr     my_fdtoiocb
        tax

	jsr	print_put_txt
	pla
	pha
	jsr	dump_hex	; dump char to write
	jsr	print_iocb_txt
	jsr	dump_iocb_num
	jsr	nl

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
	lda	#0
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

	;print_string "SER_INSTALL with error"
	;brk
        ;lda     #42
        ;ldx     #0
        ;rts

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

;----------------------------------------------------------------------------
; SER_UNINSTALL routine. Is called before the driver is removed from memory.
; Must return an SER_ERR_xx code in a/x.

SER_UNINSTALL:
	pushall
	print_string "SER_UNINSTALL called"
	pullall
        jmp     SER_CLOSE


.macro print_string2 addr, len
        ldx     #0              ; channel 0
        lda     #<addr
        sta     ICBAL,x         ; address
        lda     #>addr
        sta     ICBAH,x
        lda     #<len
        sta     ICBLL,x         ; length
        lda     #>len
        sta     ICBLH,x
        lda     #PUTCHR
        sta     ICCOM,x
        jsr     CIOV
.endmacro

.macro	push_ptr1
	lda	ptr1+1
	pha
	lda	ptr1
	pha
.endmacro

.macro	pull_ptr1
	pla
	sta	ptr1
	pla
	sta	ptr1+1
.endmacro

.macro	push_ptr2und1
	lda	ptr2+1
	pha
	lda	ptr2
	pha
	lda	ptr1+1
	pha
	sta	ptr2+1
	tax
	lda	ptr1
	pha
	sta	ptr2
.endmacro

.macro	pull_ptr2und1
	pla
	sta	ptr1
	pla
	sta	ptr1+1
	pla
	sta	ptr2
	pla
	sta	ptr2+1
.endmacro


nl_txt:  .byte   ATEOL
nl_txt_len = * - nl_txt

nl:	pushall
	print_string2 nl_txt, nl_txt_len
	pullall
	rts

iocb_txt:  .byte   " IOCB number: " 
iocb_txt_len = * - iocb_txt

print_iocb_txt:
	pushall
	print_string2 iocb_txt, iocb_txt_len
	pullall
	rts

open_txt:  .byte   "OPEN" 
open_txt_len = * - open_txt

print_open_txt:
	pushall
	print_string2 open_txt, open_txt_len
	pullall
	rts


put_txt:  .byte   "PUT " 
put_txt_len = * - put_txt

print_put_txt:
	pushall
	print_string2 put_txt, put_txt_len
	pullall
	rts

aux1_txt:  .byte   "AUX1: " 
aux1_txt_len = * - aux1_txt

dump_aux1:
	pushall
	print_string2 aux1_txt,aux1_txt_len
	tsx
	lda	$103,x
	jsr	dump_hex_low
	jsr	nl
	pullall
	rts

dump_iocb_num:
	pushall
	txa
	jsr	dump_hex_low
	pullall
	rts

dump_hex:
	pushall
	tsx
	lda	$103,x
	jsr	dump_hex_low
	pullall
	rts


; no need to preserve regs
dump_hex_low:
	tax
	push_ptr1

	lda	#<outbuf
	sta	ptr1
	lda	#>outbuf
	sta	ptr1+1

	txa
	jsr	hex8

	ldx     #0              ; channel 0

	lda	#2
        sta     ICBLL,x         ; length
	lda	#0
        sta     ICBLH,x

        lda     #<outbuf
        sta     ICBAL,x         ; address
        lda     #>outbuf
        sta     ICBAH,x
        lda     #PUTCHR
        sta     ICCOM,x
        jsr     CIOV


	pull_ptr1
	rts


;	ldy     #SER_PARAMS::BAUDRATE
;	lda	(ptr1),y

dump:
	pushall
	push_ptr2und1

.ifndef __ATARIXL__
	tay
	lda	#<outbuf
	sta	ptr1
	lda	#>outbuf
	sta	ptr1+1

	; ptr1 - pointer to string buffer
	; ptr2 - pointer to rs232 params
	tya
	jsr	hex16
	lda	#':'
	sta	(ptr1),y
	iny
	lda	#' '
	sta	(ptr1),y
	iny
	lda	#' '
	sta	(ptr1),y
	lda	ptr1
	clc
	adc	#3
	sta	ptr1
	bcc	@f
	inc	ptr1+1
@f:

.repeat 5

	ldy	#0
	lda	(ptr2),y
	jsr	hex8
	lda	#' '
	sta	(ptr1),y
	inc	ptr1
	bne	*+4
	inc	ptr1+1
	inc	ptr2
	bne	*+4
	inc	ptr2+1

.endrepeat

	lda	#ATEOL
	sta	(ptr1),y
	inc	ptr1
	bne	*+4
	inc	ptr1+1

	ldx     #0              ; channel 0

	lda	ptr1
	sec
	sbc	#<outbuf
        sta     ICBLL,x         ; length
	lda	ptr1+1
	sbc	#>outbuf
        sta     ICBLH,x

        lda     #<outbuf
        sta     ICBAL,x         ; address
        lda     #>outbuf
        sta     ICBAH,x
        lda     #PUTCHR
        sta     ICCOM,x
        jsr     CIOV
.endif

	pull_ptr2und1
	pullall
	rts


; enable concurrent rs232 mode
; gets iocb index in X
; all registers destroyed

.proc   ena_cm

        lda     #40             ; XIO 40, start concurrent IO
        sta     ICCOM,x
        sta     cm_run          ; indicate concurrent mode is running
        lda     #0
        sta     ICAX1,x
        sta     ICAX2,x
        lda     #<recv_buf
        sta     ICBAL,x
        lda     #>recv_buf
        sta     ICBAH,x
        lda     #<RECVBUF_SZ
        sta     ICBLL,x
        lda     #>RECVBUF_SZ
        sta     ICBLH,x
        lda     #$0D            ; value from 850 man, p62.  must be $0D?,
        sta     ICAX1,x         ;  or any non-zero?
        jmp     my_CIOV

.endproc        ;ena_cm

;*****************************************************************************
;* Unterprogramm:	HEX16						     *
;* Aufgabe:		16-bit Binärzahl in String wandeln (hexadezimal)     *
;* Übergabe:		ptr1   - Zeiger auf 4-byte Zielpuffer		     *
;*			AX     - zu wandelnde Zahl (high X, low A)	     *
;* Zurück:		ptr1   - Zeiger hinter Hexstring		     *
;*			Y      - 0					     *
;* Benutzt:		HEX8						     *
;*									     *
;* alle Register zerstört						     *
;*****************************************************************************

hex16:
	pha
	txa
	jsr	hex8
	pla
	;fall into hex8


;*****************************************************************************
;* Unterprogramm:	HEX8						     *
;* Aufgabe:		8-bit Binärzahl in String wandeln (hexadezimal)      *
;* Übergabe:		ptr1   - Zeiger auf 2-byte Zielpuffer		     *
;*			A      - zu wandelnde Zahl			     *
;* Zurück:		ptr1   - Zeiger hinter Hexstring		     *
;*			Y      - 0					     *
;*									     *
;* alle Register zerstört						     *
;*****************************************************************************

hex8:
	tax
	lsr	a
	lsr	a
	lsr	a
	lsr	a
	cmp	#10
	bcc	hex_1
	clc
	adc	#'A'-10
	bne	hex_2

hex_1:	adc	#'0'
hex_2:	ldy	#0
	sta	(ptr1),y
	inc	ptr1
	bne	hex_3
	inc	ptr1+1

hex_3:	txa
	and	#15

	cmp	#10
	bcc	hex_4
	clc
	adc	#'A'-10
	bne	hex_5
hex_4:	adc	#'0'
hex_5:	ldy	#0
	sta	(ptr1),y
	inc	ptr1
	bne	hex_6
	inc	ptr1+1
hex_6:	rts

.data

outbuf:	.res	48
