;
; Ullrich von Bassewitz, 06.08.1998
;
; char cgetc (void);
;

	.export		_cgetc
        .constructor    initcgetc
        .destructor     donecgetc

	.import		cursor

	.include	"c128.inc"

;--------------------------------------------------------------------------

_cgetc:	lda	KEY_COUNT 	; Get number of characters
	bne	L2	  	; Jump if there are already chars waiting

; Switch on the cursor if needed

	lda	cursor
       	beq	L1
	jsr	CURS_ON
	jmp	L2
L1:    	lda	#$01
	jsr	CURS_OFF
L2:    	lda	KEY_COUNT	; Check characters again
	beq	L2
	jsr	CURS_OFF	; Switch cursor off, if characters available

       	jsr	KBDREAD		; Read char and return in A
	ldx	#0
	rts

;--------------------------------------------------------------------------
; Module constructor/destructor

.bss
keyvec:	.res	2

.code
initcgetc:

; Save the old vector

	lda	KeyStoreVec
	sta	keyvec
	lda	KeyStoreVec+1
	sta	keyvec+1

; Set the new vector. I can only hope that this works for other C128
; versions...

	lda	#<$C6B7
	ldx	#>$C6B7

SetVec:	sei
	sta	KeyStoreVec
	stx	KeyStoreVec+1
	cli
	rts

donecgetc:
        lda     #$00
        sta     SCROLL
	lda	keyvec
	ldx	keyvec+1
	bne	SetVec

