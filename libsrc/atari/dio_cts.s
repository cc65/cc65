;
; Christian Groessler, October 2000
;
; this file provides the _dio_chs_to_snum function
;
; on the Atari this function is a dummy, it ignores
; cylinder and head 0 and returns as sector number the
; sector number it got
;
; _sectnum_t __fastcall__ _dio_chs_to_snum(unsigned int cyl, unsigned int head, unsigned int sector);
;
; _driveid_t - 8bit
; _sectnum_t - 16bit
;

 	.export		__dio_chs_to_snum
	.import		addsp4
	.include	"atari.inc"

.proc	__dio_chs_to_snum

	pha			; save sector value
	txa
	pha
	jsr	addsp4		; ignore other parameters
	pla
	tay
	pla
	rts

.endproc

