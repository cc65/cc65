;
; Christian Groessler, October 2000
;
; This file provides the __dio_open and __dio_close functions
; Since on the Atari no real open and close is necessary, they
; do not much. The __dio_open sets the sector size of the
; drive which is later used by the _dio_read and _dio_write
; functions.
;
; _dhandle_t    __fastcall__ _dio_open  (_driveid_t drive_id);
; unsigned char __fastcall__ _dio_close (_dhandle_t handle);
;

 	.export		__dio_open,__dio_close
	.export		sectsizetab
	.import		__oserror
	.importzp	ptr1,tmp1
	.include	"atari.inc"


.bss

sectsizetab:
	.res	NUMDRVS * sst_size

.code


.proc	__dio_open

	cmp	#NUMDRVS	; valid drive id?
	bcs	_inv_drive
	tay			; drive #
	asl	a		; make index from drive id
	asl	a
	tax
	lda	#128		; currently hardcoded (until I get an 815 :-)
	sta	sectsizetab+sst_sectsize,x
	sta	sectsizetab+sst_flag,x		; set flag that drive is "open"
	lda	#0
	sta	sectsizetab+sst_sectsize+1,x
	sta	__oserror			; success
	tya
	sta	sectsizetab+sst_driveno,x
;	lda	#SSTIDVAL
;	sta	sectsizetab+sst_id+1,x
	stx	tmp1
	lda	#<sectsizetab
	clc
	adc	tmp1
	sta	tmp1
	lda	#>sectsizetab
	adc	#0
	tax
	lda	tmp1
	rts

_inv_drive:
	lda	#NONDEV		; non-existent device
	sta	__oserror
	lda	#0
	tax
	rts			; return NULL

.endproc

.proc	__dio_close

	sta	ptr1
	stx	ptr1+1
	lda	#0
	ldy	#sst_flag
	sta	(ptr1),y
;	ldy	#sst_id
;	sta	(ptr1),y
	sta	__oserror	; success
	tax
	rts			; return no error

.endproc

