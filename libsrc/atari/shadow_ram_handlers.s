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


.segment "LOWCODE"

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


my_CIOV:
	pha
	enable_rom
	pla
	jsr	CIOV_org
	php
	pha
	disable_rom
	pla
	plp
	rts

my_SIOV:
	pha
	enable_rom
	pla
	jsr	SIOV_org
	php
	pha
	disable_rom
	pla
	plp
	rts

KEYBDV_wrapper:
	lda	#>(kret-1)
	pha
	lda	#<(kret-1)
	pha
	enable_rom
	lda     KEYBDV+5
        pha
        lda     KEYBDV+4
        pha
	rts		; call keyboard handler
kret:	pha
	disable_rom
	pla
	rts

.endif	; .if .defined(__ATARIXL__)
