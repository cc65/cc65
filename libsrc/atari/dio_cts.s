;
; Christian Groessler, October 2000
;
; this file provides the _dio_chs_to_snum function
;
; on the Atari this function is a dummy, it ignores
; cylinder and head 0 and returns as sector number the
; sector number it got
;
; _sectnum_t __fastcall__ _dio_chs_to_snum(_dhandle_t handle,
;                                          unsigned int cyl,
;                                          unsigned int head,
;                                          unsigned int sector);
; _dhandle_t - 16bit (ptr)
; _sectnum_t - 16bit
;

 	.export		__dio_chs_to_snum
	.import		addsp4,popax
	.importzp	ptr1
	.include	"atari.inc"

.proc	__dio_chs_to_snum

	pha			; save sector value
	txa
	pha
	jsr	addsp4		; ignore other parameters

	jsr	popax
	sta	ptr1
	stx	ptr1+1
	ldy	#sst_flag
	lda	(ptr1),y
	and	#128
	beq	_inv_hand	; handle not open or invalid

	pla
	tay
	pla
	rts

_inv_hand:
	pla
	pla
	ldx	#255
	txa
	rts

.endproc

