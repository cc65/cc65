

        .include "atari5200.inc"
	.export	setcursor
	.import	cursor	; from conio/_cursor.s



.proc   setcursor

	rts

.if 0
        ldy     #0

        lda     ROWCRS_5200
        jsr     mul20
        clc
;        adc     SAVMSC          ; add start of screen memory
;        sta     OLDADR
        txa
;        adc     SAVMSC+1
;        sta     OLDADR+1
        lda     COLCRS_5200
;        adc     OLDADR
;        sta     OLDADR
        bcc     nc
;        inc     OLDADR+1
nc:;;;     lda     (OLDADR),y
;        sta     OLDCHR

        ldx     cursor          ; current cursor setting as requested by the user
        beq     off
        ldx     #0
        beq     cont

off:    inx
cont:;;;   stx     CRSINH          ; update system variable

        beq     turnon
        and     #$7f            ; clear high bit / inverse flag
finish: ;;;sta     (OLDADR),y      ; update on-screen display
        rts

turnon: ora     #$80            ; set high bit / inverse flag
        bne     finish
.endif

.endproc
