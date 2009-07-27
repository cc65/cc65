;
; Stefan Haubenthal, 27.7.2009
;
; time_t _systime (void);
; /* Similar to time(), but:
;  *   - Is not ISO C
;  *   - Does not take the additional pointer
;  *   - Does not set errno when returning -1
;  */
;

        .include        "time.inc"
        .include        "c64.inc"

	.importzp		tmp1, tmp2

.code

; Jan 1st 1970, CIA #1 TOD
.proc	__systime

	lda	#70
	sta	TM + tm::tm_year
	lda	#1
	sta	TM + tm::tm_mday
	lda	CIA1_TODHR
	bpl	AM
	and	#%01111111
	sed
	clc
	adc	#$12
	cld
AM:	jsr	BCD2dec
	sta	TM + tm::tm_hour
	lda	CIA1_TODMIN
	jsr	BCD2dec
	sta	TM + tm::tm_min
	lda	CIA1_TODSEC
	jsr	BCD2dec
	sta	TM + tm::tm_sec
	lda	CIA1_TOD10              ; Dummy read to unfreeze
	lda	#<TM
	ldx	#>TM
	jmp	_mktime

; dec = (((BCD>>4)*10) + (BCD&0xf))
BCD2dec:tax
	and	#%00001111
	sta	tmp1
	txa
        and     #%11110000      ; *16
        lsr                     ; *8
        sta     tmp2
        lsr
        lsr                     ; *2
        adc     tmp2            ; = *10
        adc     tmp1
        rts

.endproc

.bss

TM:	.tag	tm
