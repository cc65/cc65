;
; Oliver Schmidt, 08.03.2004
;
; Identify DOS version we're running on as one of these:
;
; AppleDOS 3.3	$00
; ProDOS 1.0.1	$10
; ProDOS 1.0.2	$10
; ProDOS 1.1.1	$11
; ProDOS 1.2	$12
; ProDOS 1.3	$13
; ProDOS 1.4	$14
; ProDOS 1.5	$15
; ProDOS 1.6	$16
; ProDOS 1.7	$17
; ProDOS 1.8	$18
; ProDOS 1.9	$18
; ProDOS 2.0.1  $21
; ProDOS 2.0.2  $22
; ProDOS 2.0.3  $23

	.constructor	initdostype
	.export		__dos_type

	.include	"apple2.inc"

; Identify DOS version according to:
; - "Beneath Apple ProDOS", chapter 6-63
; - "Apple II ProDOS 8 TechNote #023: ProDOS 8 Changes and Minutia"
; - ProDOS TechRefMan, chapter 5.2.4

.segment        "INIT"

initdostype:
	lda	MLI
	cmp	#$4C		; Is MLI present? (JMP opcode)
	bne	done
	lda	$BFFF		; ProDOS KVERSION
	cmp	#$10
	bcs	:+
	ora	#$10		; Make high nibble match major version
:	sta	__dos_type
done:	rts

.bss

__dos_type:	.res	1
