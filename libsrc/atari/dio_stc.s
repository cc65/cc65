;
; Christian Groessler, October 2000
;
; this file provides the _dio_snum_to_chs function
;
; on the Atari this function is a dummy, it returns
; cylinder and head 0 and as sector the sectnum it got
;
; void       __fastcall__ _dio_snum_to_chs(_dhandle_t handle,
;                                          _sectnum_t sect_num,
;                                          unsigned int *cyl,
;                                          unsigned int *head,
;                                          unsigned int *sector);
; _dhandle_t - 16bit (ptr)
; _sectnum_t - 16bit
;

 	.export		__dio_snum_to_chs
	.include	"atari.inc"
	.importzp	ptr1,ptr2
	.import		popax,addsp2

.proc	__dio_snum_to_chs

	sta	ptr1
	stx	ptr1+1		; save pointer to sector #
	jsr	popax		; get pointer to head
	sta	ptr2
	stx	ptr2+1
	lda	#0
	tay
	sta	(ptr2),y	; set head number
	iny
	sta	(ptr2),y
	jsr	popax		; get pointer to cyl
	sta	ptr2
	stx	ptr2+1
	dey
	tya
	sta	(ptr2),y	; set cylinder number
	iny
	sta	(ptr2),y
	jsr	popax		; get sector #

	dey
	sta	(ptr1),y
	iny
	txa
	sta	(ptr1),y

	jsr	addsp2

	rts

.endproc

