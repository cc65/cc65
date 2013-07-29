;
; Atari XL shadow RAM handlers
;
; Christian Groessler, chris@groessler.org, 2013
;

DEBUG	=	1

.if .defined(__ATARIXL__)

        .include        "atari.inc"
	.include	"save_area.inc"
        .include        "zeropage.inc"
	.import		__CHARGEN_START__

	.export		sram_init
	.export		KEYBDV_wrapper

BUFSZ		=	128
BUFSZ_CIO	=	BUFSZ
BUFSZ_SIO	=	BUFSZ

.macro	disable_rom
	lda	PORTB
	and	#$fe
	sta	PORTB
	lda	#>__CHARGEN_START__
	sta	CHBAS
	sta	CHBASE
.endmacro
.macro	enable_rom
	lda	PORTB
	ora	#1
	sta	PORTB
	lda	#$E0
	sta	CHBAS
	sta	CHBASE
.endmacro

.segment "INIT"

;enable_count:	.res	1

; Turn off ROMs, install system and interrupt wrappers, set new chargen pointer

sram_init:

; disable all interrupts
	sei
	ldx	#0
	stx	NMIEN		; disable NMI

; disable ROMs
	disable_rom

; setup interrupt vectors
	lda	#<my_IRQ_han
	sta	$fffe
	lda	#>my_IRQ_han
	sta	$ffff

	lda	#<my_RESET_han
	sta	$fffc
	lda	#>my_RESET_han
	sta	$fffd

	lda	#<my_NMI_han
	sta	$fffa
	lda	#>my_NMI_han
	sta	$fffb

; setup pointers to CIOV and SIOV wrappers
	lda	#$4C		; JMP opcode
	sta	CIOV
	lda	#<my_CIOV
	sta	CIOV+1
	lda	#>my_CIOV
	sta	CIOV+2
	lda	#$4C		; JMP opcode
	sta	SIOV
	lda	#<my_SIOV
	sta	SIOV+1
	lda	#>my_SIOV
	sta	SIOV+2

; enable interrupts
	lda	#$40
	sta	NMIEN
	cli

	rts

.segment        "EXTZP" : zeropage

zpptr1:	.res	2


.segment "LOWBUFS"

; bounce buffers for CIO and SIO calls
CIO_buffer:	.res	BUFSZ_CIO
SIO_buffer:	.res	BUFSZ_SIO


.segment "LOWCODE"


; Interrupt handlers
; ------------------

; The interrupt handlers don't look at the current state of PORTB and
; unconditionally disable the ROMs on exit.
; Please note that this works, since if the ROMs are enabled we anyway
; aren't being called here because the vectors are pointing to their
; original ROM locations.

.macro	int_wrap orgvec
	.local	ret
	pha
	enable_rom
	lda	#>ret
	pha
	lda	#<ret
	pha
	php
	jmp	(orgvec)
ret:	disable_rom
	pla
	rti
.endmacro

my_IRQ_han:
.ifdef DEBUG
	php
	pha
	tya
	pha
	ldy	#0
	lda	(SAVMSC),y
	clc
	adc	#1
	sta	(SAVMSC),y
	pla
	tay
	pla
	plp
.endif
	int_wrap $FFFE

my_NMI_han:
.ifdef DEBUG
	php
	pha
	tya
	pha
	ldy	#39
	lda	(SAVMSC),y
	clc
	adc	#1
	sta	(SAVMSC),y
	pla
	tay
	pla
	plp
.endif
; set I bit to interrupted value
	pha
	txa
	pha
	tsx
	lda	$103,x
	pha
	plp
	pla
	tax
	pla
	int_wrap $FFFA

my_RESET_han:
	enable_rom
	jmp	($FFFC)


; System request handlers
; -----------------------


; for filenames we assume they will fit into our bounce buffer

; one filename, terminated by "invalid character", located at ICBAL/ICBAH

CIO_filename:
	jsr	setup_zpptr1_y0
	jsr	copy_filename
CIO_fn_cont:
	jsr	ciobuf_to_iocb
	ldy	CIO_y
	jsr	CIO_call_a		; call CIO (maybe A isn't needed, then we could call CIO_call)
	php
	pha
	jsr	restore_icba		; restore original ICBAL/ICBAH
	pla
	plp
	rts				; back to application


; two filenames, terminated and separated by "invalid character", located at ICBAL/ICBAH

CIO_filename2:
	jsr	setup_zpptr1_y0
	jsr	copy_filename
	iny
	jsr	copy_filename
	jmp	CIO_fn_cont



CIO_call_a:
	lda	CIO_a

CIOV_call:
	pha
	lda	PORTB
	sta	cur_CIOV_PORTB
	enable_rom
	pla
	jsr	CIOV_org
	php
	pha
	lda	cur_CIOV_PORTB
	sta	PORTB
	pla
	plp
	rts


; CIO handler
; We have buffer pointer and length entries in the IOCB, but their
; usage depends on the function.
; Some functions don't care about any of them (pointer and length),
; and some only use the pointer (like e.g. OPEN), and some use both.
; So we need function specific handlers to correctly deal with
; buffers which are overlapping with the ROM area.
;
; FIXME: Currently only the requests used by the runtime lib are handled.

my_CIOV:

; @@@ TODO: check X for valid IOCB index ((X < $80) and ((X & $F) == 0))

	sta	CIO_a
	sty	CIO_y
	stx	CIO_x

	lda	ICCOM,x			; get function
	cmp	#OPEN
	beq	CIO_filename		; filename as input parameter in buffer, length not used
	cmp	#PUTREC
	bcc	CIO_read		; input (GETREC or GETCHR)
	cmp	#CLOSE
	bcc	CIO_write_jmp		; output (PUTREC or PUTCHR)
	beq	CIO_call_a		; pass through, buffer not used
	cmp	#RENAME			; 2 filenames as input parameters in buffer, length not used
	beq	CIO_filename2
	cmp	#GETCWD
	bcc	CIO_filename		; filename as input parameter in buffer, length not used
	beq	CIO_invalid		; GETCWD not supported yet
	bcs	CIO_call_a		; other commands: assume no buffer
; not reached

CIO_write_jmp:
	jmp	CIO_write

CIO_invalid:
	lda	CIO_a
	ldy	#DINVCM
	rts

; READ handler
; ------------

CIO_read:
	lda	ICBLL,x
	ora	ICBLH,x
	beq	CIO_call_a		; special I/O through A register in case buffer length is 0

; @@@ TODO: check if bounce buffer is really needed because buffer is in ROM area

; If the data length is larger than our bounce buffer, we have to split the request into smaller ones.
; Otherwise we can get away with one call and a copy to the final destination afterwards.

	lda	ICBLH,x			; get high byte of length
	bne	big_read		; not zero -> data too large for our buffers
					; CHANGE HERE TO SUPPORT BOUNCE BUFFERS > 255 BYTES
	lda	#<BUFSZ_CIO
	cmp	ICBLL,x
	bcc	big_read

; Data size fits into bounce buffer

	jsr	setup_zpptr1
	jsr	ciobuf_to_iocb
	jsr	CIO_call_a		; call CIO
	php
	bpl	@no_err
	cpy	#EOFERR
	beq	@no_err
	pha
	jsr	restore_icba
	pla
	plp
	rts				; return with error

@no_err:
	sta	CIO_a
	sty	CIO_y

	jsr	copy_to_user		; copy data into user buffer
	jsr	restore_icba

	lda	CIO_a
	ldy	CIO_y
	plp
	rts				; return with success

; Data size does not fit into bounce buffer

big_read:
	lda	#0
	sta	retlen			; initialize return length
	sta	retlen+1
	jsr	iocblen_to_orig_len
	jsr	iocbptr_to_orig_ptr
	jsr	setup_zpptr1
	jsr	ciobuf_to_iocb		; let ICBAL/ICBAH point to bounce buffer

br_loop:
	jsr	cmp_orig_len_cio_bufsz	; is transfer length > bounce buffer size?
	bcs	br_last			; no, last transfer, use remaining size

	lda	#>BUFSZ_CIO
	sta	ICBLH,x			; set data length
	lda	#<BUFSZ_CIO
	sta	ICBLL,x
	bne	br_cont

br_last:
	lda	orig_len+1
	sta	ICBLH,x			; set data length
	lda	orig_len
	sta	ICBLL,x

br_cont:
	sta	req_len			; remember length of this request
	lda	ICBLH,x
	sta	req_len+1
	jsr	CIO_call_a		; do the request
	php
	bpl	br_no_err
	cpy	#EOFERR
	beq	br_no_err

	pha
	jsr	restore_icba
	pla
	plp
	rts				; return with error

br_no_err:
	sta	CIO_a
	sty	CIO_y
	pla
	sta	CIO_p
	jsr	copy_to_user

; update retlen
	clc
	lda	retlen
	adc	ICBLL,x
	sta	retlen
	lda	retlen+1
	adc	#0
	sta	retlen+1

; if the request read less bytes than requested, we're done
	lda	ICBLL,x
	cmp	req_len
	bne	br_done
	lda	ICBLH,x
	cmp	req_len+1
	bne	br_done

; update user buffer pointer (zpptr1)
	clc
	lda	zpptr1
	adc	ICBLL,x
	sta	zpptr1
	lda	zpptr1+1
	adc	#0
	sta	zpptr1+1

; update remaining length
	sec
	lda	orig_len
	sbc	ICBLL,x
	sta	orig_len
	lda	orig_len+1
	sbc	#0
	sta	orig_len+1

; still something left to do (remaining length != 0)?
	lda	orig_len
	ora	orig_len+1
	beq	br_done
	jmp	br_loop

; done, write original buffer pointer and total transfer length to IOCB and return to application
br_done:
	lda	retlen
	sta	ICBLL,x
	lda	retlen+1
	sta	ICBLH,x
	jsr	orig_ptr_to_iocbptr
	lda	CIO_p
	pha
	lda	CIO_a
	ldy	CIO_y
	plp
	rts				; return with success



CIO_call_a_jmp:
	jmp	CIO_call_a



; WRITE handler
; -------------


CIO_write:
	lda	ICBLL,x
	ora	ICBLH,x
	beq	CIO_call_a_jmp		; special I/O through A register in case buffer length is 0

; @@@ TODO: check if bounce buffer is really needed because buffer is in ROM area

; If the data length is larger than our bounce buffer, we have to split the request into smaller ones.
; Otherwise we can get away with a copy to the bounce buffer and the call.

	lda	ICBLH,x			; get high byte of length
	bne	big_write		; not zero -> data too large for our buffers
					; CHANGE HERE TO SUPPORT BOUNCE BUFFERS > 255 BYTES
	lda	#<BUFSZ_CIO
	cmp	ICBLL,x
	bcc	big_write


; Data size fits into bounce buffer

	jsr	setup_zpptr1
	jsr	ciobuf_to_iocb
	jsr	copy_from_user
	ldy	CIO_y
	jsr	CIO_call_a
	php
	pha
	jsr	restore_icba
	pla
	plp
	rts				; return to application


; Data size does not fit into bounce buffer

big_write:
	lda	#0
	sta	retlen			; initialize return length
	sta	retlen+1
	jsr	iocblen_to_orig_len
	jsr	iocbptr_to_orig_ptr
	jsr	setup_zpptr1
	jsr	ciobuf_to_iocb		; let ICBAL/ICBAH point to bounce buffer

bw_loop:
	jsr	cmp_orig_len_cio_bufsz	; is transfer length > bounce buffer size?
	bcs	bw_last			; no, last transfer, use remaining size

	lda	#>BUFSZ_CIO
	sta	ICBLH,x			; set data length
	lda	#<BUFSZ_CIO
	sta	ICBLL,x
	bne	bw_cont

bw_last:
	lda	orig_len+1
	sta	ICBLH,x			; set data length
	lda	orig_len
	sta	ICBLL,x

bw_cont:
	sta	req_len			; remember length of this request
	lda	ICBLH,x
	sta	req_len+1
	jsr	copy_from_user
	jsr	CIO_call_a		; do the request
	php
	bpl	bw_no_err

	plp
	rts				; error return

bw_no_err:
	sta	CIO_a
	sty	CIO_y
	pla
	sta	CIO_p

; update retlen
	clc
	lda	retlen
	adc	ICBLL,x
	sta	retlen
	lda	retlen+1
	adc	#0
	sta	retlen+1

; if the request wrote less bytes than requested, we're done
	lda	ICBLL,x
	cmp	req_len
	bne	bw_done
	lda	ICBLH,x
	cmp	req_len+1
	bne	bw_done

; update user buffer pointer (zpptr1)
	clc
	lda	zpptr1
	adc	ICBLL,x
	sta	zpptr1
	lda	zpptr1+1
	adc	#0
	sta	zpptr1+1

; update remaining length
	sec
	lda	orig_len
	sbc	ICBLL,x
	sta	orig_len
	lda	orig_len+1
	sbc	#0
	sta	orig_len+1

; still something left to do (remaining length != 0)?
	lda	orig_len
	ora	orig_len+1
	beq	bw_done
	jmp	bw_loop

bw_done:
	lda	retlen
	sta	ICBLL,x
	lda	retlen+1
	sta	ICBLH,x
	jsr	orig_ptr_to_iocbptr
	lda	CIO_p
	pha
	lda	CIO_a
	ldy	CIO_y
	plp
	rts				; return with success



; check if length is larger than bounce buffer size
; input:   orig_len - length
; output:         A - destroyed
;                CF - 0/1 for larger/not larger
cmp_orig_len_cio_bufsz:
	sec
	lda	#<BUFSZ_CIO
	sbc	orig_len
	lda	#>BUFSZ_CIO
	sbc	orig_len+1
	rts


; copy data from bounce buffer into user buffer
; input:   X - IOCB index
;     zpptr1 - pointer to user buffer
; output:  A - destroyed
;	   Y - 0
copy_to_user:
	ldy	ICBLL,x			; get # of bytes read (CHANGE HERE TO SUPPORT BOUNCE BUFFERS > 255 BYTES)
	beq	@copy_done
@copy:	dey
	lda	CIO_buffer,y
	sta	(zpptr1),y
	cpy	#0
	bne	@copy
@copy_done:
	rts


; copy data from user buffer into bounce buffer
; input:   X - IOCB index
;     zpptr1 - pointer to user buffer
; output:  A - destroyed
;	   Y - 0
copy_from_user:
	ldy	ICBLL,x			; get # of bytes to write (CHANGE HERE TO SUPPORT BOUNCE BUFFERS > 255 BYTES)
	beq	@copy_done
@copy:	dey
	lda	(zpptr1),y
	sta	CIO_buffer,y
	cpy	#0
	bne	@copy
@copy_done:
	rts


; copy ICBLL/ICBLH to 'orig_len'
; input:   X - IOCB index
; output:  A - destroyed
iocblen_to_orig_len:
	lda	ICBLL,x
	sta	orig_len
	lda	ICBLH,x
	sta	orig_len+1
	rts


; copy ICBAL/ICBAH to 'orig_ptr'
; input:   X - IOCB index
; output:  A - destroyed
iocbptr_to_orig_ptr:
	lda	ICBAL,x
	sta	orig_ptr
	lda	ICBAH,x
	sta	orig_ptr+1
	rts


; copy 'orig_ptr' to ICBAL/ICBAH
; input:   X - IOCB index
; output:  A - destroyed
orig_ptr_to_iocbptr:
	lda	orig_ptr
	sta	ICBAL,x
	lda	orig_ptr+1
	sta	ICBAH,x
	rts


; restore original contents of ICBAL/ICBAH from 'zpptr1'
; input:   X - IOCB index
; output:  A - destroyed
restore_icba:
	lda	zpptr1
	sta	ICBAL,x
	lda	zpptr1+1
	sta	ICBAH,x
	rts


; put bounce buffer address into ICBAL/ICBAH
; input:   X - IOCB index
; output:  A - destroyed
ciobuf_to_iocb:
	lda	#<CIO_buffer
	sta	ICBAL,x
	lda	#>CIO_buffer
	sta	ICBAH,x
	rts


; copy file name pointed to by 'zpptr1' to bounce buffer 'CIO_buffer'
; input:   Y - index into file name buffer and CIO_buffer
; output:  Y - points to first invalid byte after file name
;          A - destroyed
copy_filename:
	lda	(zpptr1),y
	sta	CIO_buffer,y
	beq	copy_fn_done
	iny
	cmp	#ATEOL
	bne	copy_filename
	dey
copy_fn_done:
	rts


; write IOCB buffer address into zpptr1
; input:   X - IOCB index
; output:  Y - 0 (for setup_zpptr1_y0, else unchanged)
;          A - destroyed
setup_zpptr1_y0:
	ldy	#0
setup_zpptr1:
	lda	ICBAL,x			; put buffer address into zp pointer
	sta	zpptr1
	lda	ICBAH,x
	sta	zpptr1+1
	rts

;---------------------------------------------------------

my_SIOV:
	pha
	lda	PORTB
	sta	cur_SIOV_PORTB
	enable_rom
	pla
	jsr	SIOV_org
	php
	pha
	lda	cur_SIOV_PORTB
	sta	PORTB
	pla
	plp
	rts

;---------------------------------------------------------

KEYBDV_wrapper:

	lda	#>(kret-1)
	pha
	lda	#<(kret-1)
	pha
	lda	PORTB
	sta	cur_KEYBDV_PORTB
	enable_rom
	lda     KEYBDV+5
        pha
        lda     KEYBDV+4
        pha
	rts		; call keyboard handler
kret:	pha
	lda	cur_KEYBDV_PORTB
	sta	PORTB
	pla
	rts

CIO_a:			.res	1
CIO_x:			.res	1
CIO_y:			.res	1
CIO_p:			.res	1
cur_CIOV_PORTB:		.res	1
cur_SIOV_PORTB:		.res	1
cur_KEYBDV_PORTB:	.res	1
orig_ptr:		.res	2
orig_len:		.res	2
req_len:		.res	2
retlen:			.res	2

.endif	; .if .defined(__ATARIXL__)
