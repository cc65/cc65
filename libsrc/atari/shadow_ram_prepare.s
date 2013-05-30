;
; Atari XL shadow RAM preparation routines
;
; Tasks:
; - move screen memory below load address
; - copy shadow RAM contents to their destination
;
; Christian Groessler, chris@groessler.org, 2013
;

.if .defined(__ATARIXL__)

	.export		sramprep
        .import         __SRPREP_LOAD__, __SRPREP_SIZE__
	.import		__SHADOW_RAM_LOAD__, __SHADOW_RAM_SIZE__
	.import		__CHARGEN_LOAD__, __CHARGEN_SIZE__
	.import		__SAVEAREA_LOAD__

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
	jsr	CIOV
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

; disable BASIC

	lda	PORTB
	ora	#2
	sta	PORTB


; ... change memory bla

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
	
	

; ... issue ar GRAPHICS 0 call (copied'n'pasted from TGI drivers)


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
        jsr     CIOV
	bpl	okoko

	print_string "GR 0 FAILED"
	jsr	delay
	jsr	delay
	jsr	delay

	jmp xxx

okoko:


        ; Now close it again; we don't need it anymore :)
        lda     #CLOSE
        sta     ICCOM,x
        jsr     CIOV

	print_string "GR 0 OKOKO"
	jsr	delay






xxx:





        rts

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
; Chunk "trailer" - sets INITAD

.segment        "SRPREPTRL"

        .word   INITAD
        .word   INITAD+1
        .word   __SRPREP_LOAD__

.endif	; .if .defined(__ATARIXL__)
