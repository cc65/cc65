;
; Atari XL shadow RAM handlers
;
; Christian Groessler, chris@groessler.org, 2013
;


.if .defined(__ATARIXL__)

        .include        "atari.inc"
	.include	"save_area.inc"
	.import		__CHARGEN_START__

	.export		sram_init
	.export		KEYBDV_wrapper

BUFSZ		=	128
BUFSZ_CIO	=	BUFSZ
BUFSZ_SIO	=	BUFSZ

.macro	disable_rom
	.local	cont
	;dec	enable_count
	;bne	cont
	lda	PORTB
	and	#$fe
	sta	PORTB
	lda	#>__CHARGEN_START__
	sta	CHBAS
	sta	CHBASE
cont:
.endmacro
.macro	enable_rom
	;inc	enable_count
	lda	PORTB
	ora	#1
	sta	PORTB
	lda	#$E0
	sta	CHBAS
	sta	CHBASE
.endmacro

.segment "INIT"

enable_count:	.res	1

; Turn off ROMs, install system and interrupt wrappers, set new chargen pointer

sram_init:

; disable all interrupts
	sei
	ldx	#0
	stx	NMIEN		; disable NMI

; disable ROMs
	;inx
	stx	enable_count
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


.segment "LOWBUFS"

; bounce buffers for CIO and SIO calls
CIOV_buffer:	.res	BUFSZ_CIO
SIOV_buffer:	.res	BUFSZ_SIO


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
	int_wrap IRQ_save

my_NMI_han:
	int_wrap NMI_save

my_RESET_han:
	int_wrap RESET_save


; System request handlers
; -----------------------

; CIO handler
; We have buffer pointer and length entries in the IOCB, but their
; usage depends on the function.
; Some functions don't care about both (pointer and length), and some
; only use the pointer (like e.g. OPEN), and some use both.
; So we need function specific handlers to correctly deal with
; buffers which are overlapping with the ROM area.
; All input and output registers need to be preserved (I'm not 100%
; sure about Y, but let's preserve it for now.)
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
	bcc	CIO_write		; output (PUTREC or PUTCHR)
	beq	CIO_pass		; pass through, buffer not used
	cmp	#RENAME			; 2 filenames as input parameters in buffer, length not used
	beq	CIO_filename2
	cmp	GETCWD
	bcc	CIO_filename		; filename as parameter in buffer, length not used
	beq	CIO_read		; input
	bcs	CIO_pass		; other command: assume no buffers



.if 0		; all crap
; check if buffer is potentially used (buffer length != 0)

	pha
	lda	ICBLL,x
	ora	ICBLH,x
	bne	could_be
	pla
	jmp	CIOV_call

; buffer might be used by the request

could_be:

; check if buffer is inside ROM area

	lda	ICBAH,x
	cmp	#$C0			; if buffer is above $C000, it's definitely inside ROM area
	bcs	need_work

	lda	ICBAL,x			; low byte of buffer address
	adc	ICBLL,x			; low byte of buffer length
	lda	ICBAH,x			; high byte (address)
	adc	ICBLH,x			; high byte (length)
	cmp	#$C0
	bcc	CIOV_call		; no need to use bounce buffers, just forward call to CIO

need_work:

; Check if length is bigger than the size of our bounce buffer.
; If yes, we need to split the call into multiple calls.
; @@@ FIXME: currently only supports bounce buffers < 256 bytes

	lda	ICBLH,x			; high byte of length
	bne	hard_work
	lda	ICBLL,x			; low byte of length
	cmp	#<BUFSZ_CIO
	beq	little_work
	bcs	hard_work

; Request buffer is smaller or equal to our bounce buffer.
; Copy the data into the bounce buffer, set bounce buffer address
; in the IOCB, call CIO, and copy the output data back.
; @@@ TODO: check for read/write and do only one of the copy operations
little_work:



; Request buffer is larger than our bounce buffer.
hard_work:

.endif	; crap alert

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

cur_CIOV_PORTB:		.res	1
cur_SIOV_PORTB:		.res	1
cur_KEYBDV_PORTB:	.res	1

.endif	; .if .defined(__ATARIXL__)
