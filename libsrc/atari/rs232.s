;
; Christian Groessler, Dec-2001
;
; RS232 routines using the R: device (currently tested with an 850 only)
;
; unsigned char __fastcall__ rs232_init (char hacked);
; unsigned char __fastcall__ rs232_params (unsigned char params, unsigned char parity);
; unsigned char __fastcall__ rs232_done (void);
; unsigned char __fastcall__ rs232_get (char* B);
; unsigned char __fastcall__ rs232_put (char B);
; unsigned char __fastcall__ rs232_pause (void); [TODO]
; unsigned char __fastcall__ rs232_unpause (void); [TODO]
; unsigned char __fastcall__ rs232_status (unsigned char* status,
;					   unsigned char* errors); [TODO]
;

	.import		findfreeiocb
	.import		__seterrno, __do_oserror, __oserror
	.import		fddecusage
	.import		fdtoiocb
	.import		__inviocb
	.import		clriocb
	.import		newfd
	.import		_close, pushax, popax, popa
	.importzp	ptr1, tmp2, tmp3

	.export	       	_rs232_init, _rs232_params, _rs232_done, _rs232_get
	.export	      	_rs232_put, _rs232_pause, _rs232_unpause, _rs232_status

	.include	"atari.inc"
	.include	"../common/errno.inc"

	.rodata

rdev:	.byte	"R:", ATEOL, 0

	.bss

; receive buffer
RECVBUF_SZ = 256
recv_buf: .res	RECVBUF_SZ

cm_run:	.res	1	; concurrent mode running?

	.data

rshand:	.word	$ffff

	.code

;----------------------------------------------------------------------------
;
; unsigned char __fastcall__ rs232_init (char hacked);
; /* Initialize the serial port. The parameter is ignored in the Atari version.
;  * return 0/-1 for OK/Error
;  */
;

.proc	_rs232_init

	jsr	findfreeiocb
	beq	iocbok		; we found one

	lda	#<EMFILE	; "too many open files"
	ldx	#>EMFILE
seterr:	jsr	__seterrno
	lda	#$FF
	tax
	rts			; return -1

iocbok:	txa
	tay			; move iocb # into Y
	lda	#3
	sta	tmp3		; name length + 1
	lda	#<rdev
	ldx	#>rdev
	jsr	newfd
	tya
	bcs	doopen		; C set: open needed / device not already open

	pha
	jsr	_rs232_done	;** shut down if started  @@@TODO check this out!!
	pla

doopen:	tax
	pha
	jsr	clriocb
	pla
	tax
	lda	#<rdev
	sta	ICBAL,x
	lda	#>rdev
	sta	ICBAH,x
	lda	#OPEN
	sta	ICCOM,x

 	lda	#$0D		; mode in+out+concurrent
	sta	ICAX1,x
	lda	#0
	sta	ICAX2,x
	sta	ICBLL,x		; zap buf len
	sta	ICBLH,x
	jsr	CIOV
	bmi	cioerr

	lda	tmp2		; get fd
	sta	rshand
	ldx	#0
	stx	rshand+1
	txa
	stx	__oserror
	rts

cioerr:	jsr	fddecusage	; decrement usage counter of fd as open failed
	jmp	__do_oserror

.endproc	; _rs232_init


;----------------------------------------------------------------------------
;
; unsigned char __fastcall__ rs232_params (unsigned char params, unsigned char parity);
;
; Set communication parameters.
;
; params contains baud rate, stop bits and word size
; parity contains parity
;
; 850 manual documents restrictions on the baud rate (not > 300), when not
; using 8 bit word size. So only 8 bit is currently tested.
;

; shouldn't this come from a "rs232.inc" ??
ErrNotInitialized 	= $01
ErrNoData         	= $04

.proc	_rs232_params

	sta	tmp2
	lda	rshand
	cmp	#$ff
	bne	work		; work only if initialized
	lda	#ErrNotInitialized
	bne	done
work:	lda	rshand
	ldx	#0
	jsr	fdtoiocb	; get iocb index into X
	bmi	inverr		; shouldn't happen
	tax

	; set handshake lines

	lda	#34		; xio 34, set cts, dtr etc
	sta	ICCOM,x
	lda	#192+48+3	; DTR on, RTS on, XMT on
	sta	ICAX1,x
	lda	#0
	sta	ICBLL,x
	sta	ICBLH,x
	sta	ICBAL,x
	sta	ICBAH,x
	sta	ICAX2,x
	jsr	CIOV
	bmi	cioerr

	; set baud rate, word size, stop bits and ready monitoring

	lda	#36		; xio 36, baud rate
	sta	ICCOM,x
	jsr	popa		; get parameter
	sta	ICAX1,x
	;ICAX2 = 0, monitor nothing
	jsr	CIOV
	bmi	cioerr

	; set translation and parity

	lda	#38		; xio 38, translation and parity
	sta	ICCOM,x
	lda	tmp2
	ora	#32		; no translation
	sta	ICAX1,x
	jsr	CIOV
	bmi	cioerr

	lda	#0
done:	ldx	#0
	rts

inverr:	jmp	__inviocb

.endproc	;_rs232_params

cioerr:	jmp	__do_oserror


;----------------------------------------------------------------------------
;
; unsigned char __fastcall__ rs232_done (void);
; /* Close the port, deinstall the interrupt hander. You MUST call this function
;  * before terminating the program, otherwise the machine may crash later. If
;  * in doubt, install an exit handler using atexit(). The function will do
;  * nothing, if it was already called.
;  */
;

.proc	_rs232_done

	lda	rshand
	cmp	#$ff
	beq	done
work:	ldx	rshand+1
	jsr	pushax
	jsr	_close
	pha
	txa
	pha
	ldx	#$ff
	stx	rshand
	stx	rshand+1
	inx
	stx	cm_run
	pla
	tax
	pla
done:	rts

.endproc	;rs232_done


;----------------------------------------------------------------------------
;
; unsigned char __fastcall__ rs232_get (char* B);
; /* Get a character from the serial port. If no characters are available, the
;  * function will return RS_ERR_NO_DATA, so this is not a fatal error.
;  */
;

.proc	_rs232_get

	ldy	rshand
	cpy	#$ff
	bne	work		; work only if initialized
	lda	#ErrNotInitialized
	bne	nierr

work: 	sta	ptr1
 	stx	ptr1+1		; store pointer to received char

	lda	rshand
	ldx	#0
	jsr	fdtoiocb
	tax
	lda	cm_run		; concurrent mode already running?
	bne	go
	jsr	ena_cm		; turn on concurrent mode

go:	; check whether there is any input available

	lda	#STATIS		; status request, returns bytes pending
	sta	ICCOM,x
	jsr	CIOV
	bmi	cioerr		; @@@ error handling

	lda	DVSTAT+1	; get byte count pending
	ora	DVSTAT+2
	beq	nix_da		; no input waiting...

	; input is available: get it!
	
	lda	#GETCHR		; get raw bytes
	sta	ICCOM,x		; in command code
	lda	#0
	sta	ICBLL,x
	sta	ICBLH,x
	sta	ICBAL,x
	sta	ICBAH,x
	jsr	CIOV		; go get it
	bmi	cioerr		; @@@ error handling

	ldx	#0
	sta	(ptr1,x)	; return received byte
	txa
	rts
	
nierr:	ldx	#0
	rts

nix_da:	lda	#ErrNoData
   	ldx 	#0
   	rts

.endproc	;_rs232_get


;----------------------------------------------------------------------------
;
; unsigned char __fastcall__ rs232_put (char B);
; /* Send a character via the serial port. There is a transmit buffer, but
;  * transmitting is not done via interrupt. The function returns
;  * RS_ERR_OVERFLOW if there is no space left in the transmit buffer.
;  */
;

.proc	_rs232_put

	ldy	rshand
	cpy	#$ff
	bne	work		; work only if initialized
	lda	#ErrNotInitialized
	bne	nierr

work:	pha
	lda	rshand
	ldx	#0
	jsr	fdtoiocb
	tax
	lda	cm_run		; concurrent mode already running?
	bne	go
	jsr	ena_cm		; turn on concurrent mode

	; @@@TODO:	 check output buffer overflow
go:	lda	#PUTCHR		; put raw bytes
	sta	ICCOM,x		; in command code
	lda	#0
	sta	ICBLL,x
	sta	ICBLH,x
	sta	ICBAL,x
	sta	ICBAH,x
	pla			; get the char back
	jsr	CIOV		; go do it
	rts

nierr:	ldx	#0
	rts

.endproc	;_rs232_put

;----------------------------------------------------------------------------
;
; unsigned char __fastcall__ rs232_pause (void);
; /* Assert flow control and disable interrupts. */
;

_rs232_pause:


;----------------------------------------------------------------------------
;
; unsigned char __fastcall__ rs232_unpause (void);
; /* Re-enable interrupts and release flow control */
;

_rs232_unpause:


;----------------------------------------------------------------------------
;
; unsigned char __fastcall__ rs232_status (unsigned char* status,
;					   unsigned char* errors);
; /* Return the serial port status. */
;

_rs232_status:

	lda	#255
	tax
	rts


; enable concurrent rs232 mode
; gets iocb index in X
; all registers destroyed

.proc	ena_cm

	lda	#40		; XIO 40, start concurrent IO
	sta	ICCOM,x
	sta	cm_run		; indicate concurrent mode is running
	lda	#0
	sta	ICAX1,x
	sta	ICAX2,x
	lda	#<recv_buf
	sta	ICBAL,x
	lda	#>recv_buf
	sta	ICBAH,x
	lda	#<RECVBUF_SZ
	sta	ICBLL,x
	lda	#>RECVBUF_SZ
	sta	ICBLH,x
	lda	#$0D		; value from 850 man, p62.  must be 0D?,
	sta	ICAX1,x		;  or any non-zero?
	jmp	CIOV

.endproc	;ena_cm

	.end
