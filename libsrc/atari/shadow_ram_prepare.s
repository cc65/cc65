;
; Atari XL shadow RAM preparation routines
;
; Tasks:
; - move screen memory below load address
; - copy ROM chargen to its new place
; - copy shadow RAM contents to their destination
;
; Christian Groessler, chris@groessler.org, 2013
;

.if .defined(__ATARIXL__)

	.export		sramprep
        .import         __SRPREP_LOAD__, __SRPREP_SIZE__
	.import		__SHADOW_RAM_LOAD__, __SHADOW_RAM_SIZE__
	.import		__SHADOW_RAM_RUN__
	.import		__CHARGEN_START__, __CHARGEN_SIZE__
	.import		__SAVEAREA_LOAD__
	.import		zpsave

        .include        "zeropage.inc"
        .include        "atari.inc"
	.include	"save_area.inc"

.macro print_string text
	.local	start, cont
	jmp	cont
start:	.byte	text, ATEOL
cont:	ldx	#0		; channel 0
	lda	#<start
	sta	ICBAL,x		; address
	lda	#>start
	sta	ICBAH,x
	lda	#<(cont - start)
	sta	ICBLL,x		; length
	lda	#>(cont - start)
	sta	ICBLH,x
	lda	#PUTCHR
	sta	ICCOM,x
	jsr	CIOV_org
.endmacro

; ------------------------------------------------------------------------
; Chunk header

.segment        "SRPREPHDR"

        .word   __SRPREP_LOAD__
        .word   __SRPREP_LOAD__ + __SRPREP_SIZE__ - 1

; ------------------------------------------------------------------------
; Actual code

.segment        "SRPREP"

sramprep:
	print_string "in sramprep"

; save values of modified system variables and ports

	lda	RAMTOP
	sta	RAMTOP_save
	lda	MEMTOP
	sta	MEMTOP_save
	lda	MEMTOP+1
	sta	MEMTOP_save+1
	lda	APPMHI
	sta	APPMHI_save
	lda	APPMHI+1
	sta	APPMHI_save+1
	lda	PORTB
	sta	PORTB_save
	lda	CIOV		; zero-page wrapper
	sta	ZP_CIOV_save
	lda	CIOV+1
	sta	ZP_CIOV_save+1
	lda	CIOV+2
	sta	ZP_CIOV_save+2
	lda	SIOV		; zero-page wrapper
	sta	ZP_SIOV_save
	lda	SIOV+1
	sta	ZP_SIOV_save+1
	lda	SIOV+2
	sta	ZP_SIOV_save+2

	lda	$fffe
	sta	IRQ_save
	lda	$ffff
	sta	IRQ_save+1
	lda	$fffc
	sta	RESET_save
	lda	$fffd
	sta	RESET_save+1
	lda	$fffa
	sta	NMI_save
	lda	$fffb
	sta	NMI_save+1

; disable BASIC

	lda	PORTB
	ora	#2
	sta	PORTB


; ... change system memory variables bla

CMPVAL = 64+255+992		; you may ask, why these values...   @@@ document

sys_ok:	lda	#<__SAVEAREA_LOAD__
	sec
	sbc	#<CMPVAL
	sta	MEMTOP
	sta	APPMHI
	lda	#>__SAVEAREA_LOAD__
	sbc	#>CMPVAL
	sta	MEMTOP+1
	sta	APPMHI+1

	lda	#>__SAVEAREA_LOAD__ - 1
	sta	RAMTOP



; ... issue a GRAPHICS 0 call (copied'n'pasted from TGI drivers)


	ldx	#$50		; take any IOCB, hopefully free (@@@ fixme)

        ; Reopen it in Graphics 0
        lda     #OPEN
        sta     ICCOM,x
        lda     #OPNIN | OPNOT
        sta     ICAX1,x
        lda     #0
        sta     ICAX2,x
        lda     #<screen_device
        sta     ICBAL,x
        lda     #>screen_device
        sta     ICBAH,x
        lda     #<screen_device_length
        sta     ICBLL,x
        lda     #>screen_device_length
        sta     ICBLH,x
        jsr     CIOV_org
	bpl	okoko

	print_string "GR 0 FAILED"
	jsr	delay
	jsr	delay
	jsr	delay

	jmp	(DOSVEC)		; abort loading


okoko:


        ; Now close it again; we don't need it anymore :)
        lda     #CLOSE
        sta     ICCOM,x
        jsr     CIOV_org

	print_string "GR 0 OKOKO"
	jsr	delay


; Save the zero page locations we need

        ldx     #zpspace-1
L1:     lda     sp,x
        sta     zpsave,x
        dex
        bpl     L1

; copy chargen to low memory

	lda	#>(__SRPREP_LOAD__ + __SRPREP_SIZE__)
	sta	ptr3+1
	lda	#<(__SRPREP_LOAD__ + __SRPREP_SIZE__)
	sta	ptr3
	beq	cg_addr_ok

	; page align the new chargen address
	inc	ptr3+1
	lda	#0
	sta	ptr3

cg_addr_ok:
	lda	#<DCSORG
	sta	ptr1
	lda	#>DCSORG
	sta	ptr1+1
	lda	ptr3
	sta	ptr2
	lda	ptr3+1
	sta	ptr2+1
	lda	#>__CHARGEN_SIZE__
	sta	tmp2
	lda	#<__CHARGEN_SIZE__
	sta	tmp2+1
	jsr	memcopy

; TODO: switch to this temp. chargen

; disable ROMs
	sei
	ldx	#0
	stx	NMIEN		; disable NMI
	lda	PORTB
	and	#$fe
	sta	PORTB		; now ROM is mapped out

; copy shadow RAM contents to their destination

	lda	#<__SHADOW_RAM_SIZE__
	bne	do_copy
	lda	#>__SHADOW_RAM_SIZE__
	beq	no_copy				; we have no shadow RAM contents

	; ptr1 - src; ptr2 - dest; tmp1, tmp2 - len
do_copy:lda	#<__SHADOW_RAM_LOAD__
	sta	ptr1
	lda	#>__SHADOW_RAM_LOAD__
	sta	ptr1+1
	lda	#<__SHADOW_RAM_RUN__
	sta	ptr2
	lda	#>__SHADOW_RAM_RUN__
	sta	ptr2+1
	lda	#<__SHADOW_RAM_SIZE__
	sta	tmp1
	lda	#>__SHADOW_RAM_SIZE__
	sta	tmp2

	jsr	memcopy

no_copy:

; copy chargen to its new location

	lda	ptr3
	sta	ptr1
	lda	ptr3+1
	sta	ptr1+1
	lda	#<__CHARGEN_START__
	sta	ptr2
	lda	#>__CHARGEN_START__
	sta	ptr2+1
	lda	#>__CHARGEN_SIZE__
	sta	tmp2
	lda	#<__CHARGEN_SIZE__
	sta	tmp2+1
	jsr	memcopy

; re-enable ROM

	lda	PORTB
	ora	#1
	sta	PORTB
	lda	#$40
	sta	NMIEN			; enable VB again
	cli				; and enable IRQs

        rts


; my 6502 fu is rusty, so I took a routine from the internet (http://www.obelisk.demon.co.uk/6502/algorithms.html)

; copy memory
; ptr1      - source
; ptr2      - destination
; tmp2:tmp1 - len

.proc	memcopy

	ldy	#0
	ldx	tmp2
	beq	last
pagecp:	lda	(ptr1),y
	sta	(ptr2),y
	iny
	bne	pagecp
	inc	ptr1+1
	inc	ptr2+1
	dex
	bne	pagecp
last:	cpy	tmp1
	beq	done
	lda	(ptr1),y
	sta	(ptr2),y
	iny
	bne	last
done:	rts

.endproc


.byte "HERE ****************** HERE ***************>>>>>>"

sramsize:
	.word	__SHADOW_RAM_SIZE__

; short delay
.proc	delay

	lda	#10
l:	jsr	delay1
	clc
	sbc	#0
	bne	l
	rts

delay1:	ldx	#0
	ldy	#0
loop:	dey
	bne	loop
	dex
	bne	loop
	rts

.endproc

screen_device:	.byte "S:",0
screen_device_length = * - screen_device

	.byte	" ** srprep ** end-->"

; ------------------------------------------------------------------------
; Provide an empty SHADOW_RAM segment in order that the linker is happy
; if the user program doesn't have a SHADOW_RAM segment.

.segment        "SHADOW_RAM"


; ------------------------------------------------------------------------
; Chunk "trailer" - sets INITAD

.segment        "SRPREPTRL"

        .word   INITAD
        .word   INITAD+1
        .word   __SRPREP_LOAD__

.endif	; .if .defined(__ATARIXL__)
