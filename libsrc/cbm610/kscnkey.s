;
; Ullrich von Bassewitz, 28.09.1998
;
; Keyboard polling stuff for the 610.
;

 	.export	  	SCNKEY
	.importzp     	tpi2, ktab1, ktab2, ktab3, ktab4

	.include      	"cbm610.inc"


.proc	SCNKEY

        lda     #$FF
        sta     ModKey
        sta     NorKey
        lda	#$00
	sta	KbdScanBuf
	ldy	#TPI::PRB
	sta	(tpi2),y
	ldy	#TPI::PRA
	sta	(tpi2),y
        jsr     Poll
        and     #$3F
        eor     #$3F
        bne     L1
        jmp     NoKey

L1:	lda     #$FF
	ldy	#TPI::PRA
	sta	(tpi2),y
        asl     a
	ldy	#TPI::PRB
	sta	(tpi2),y
        jsr     Poll
        pha
        sta     ModKey
        ora     #$30
        bne     L3	   	; Branch always

L2:	jsr     Poll
L3:	ldx     #$05
	ldy	#$00
L4:	lsr     a
        bcc     L5
        inc	KbdScanBuf
        dex
        bpl     L4
        sec
	ldy	#TPI::PRB
  	lda	(tpi2),y
  	rol	a
  	sta	(tpi2),y
       	ldy	#TPI::PRA
  	lda	(tpi2),y
  	rol	a
  	sta	(tpi2),y
        bcs     L2
        pla
        bcc     NoKey 	   	; Branch always

L5:	ldy	KbdScanBuf
	sty     NorKey
        pla
        asl     a
        asl     a
        asl     a
        bcc     L6
        bmi     L7
        lda     (ktab2),y		; Shifted normal key
        ldx     GrafMode
        beq     L8
        lda     (ktab3),y		; Shifted key in graph mode
        bne     L8

L6:	lda     (ktab4),y		; Key with ctrl pressed
	bne	L8
L7:	lda	(ktab1),y		; Normal key
L8:	tax
	cpx     #$FF  	 		; Valid key?
        beq     Done
        cpy     LastIndex
        beq     Repeat
        ldx     #$13
        stx     RepeatDelay
        ldx     KeyIndex
        cpx     #$09
        beq     NoKey
        cpy     #$59
        bne     PutKey
        cpx     #$08
        beq     NoKey
        sta     KeyBuf,x
        inx
        bne     PutKey

NoKey:	ldy     #$FF
Done:  	sty     LastIndex
End:	lda	#$7F
	ldy	#TPI::PRA
	sta	(tpi2),y
	ldy	#TPI::PRB
	lda	#$FF
	sta	(tpi2),y
        rts

Repeat:	dec     RepeatDelay
        bpl     End
        inc     RepeatDelay
        dec     RepeatCount
        bpl     End
        inc     RepeatCount
        ldx     KeyIndex
        bne     End

PutKey:	sta     KeyBuf,x
        inx
        stx     KeyIndex
        ldx     #$03
        stx     RepeatCount
        bne     Done

.endproc


; Poll the keyboard port until it's stable

.proc	Poll
	ldy	#TPI::PRC
L1:	lda	(tpi2),y
	sta	KeySave
	lda	(tpi2),y
	cmp	KeySave
	bne	L1
	rts
.endproc


.bss

KeySave:	.res	1


