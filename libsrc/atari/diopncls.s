;
; Christian Groessler, October 2000
;
; This file provides the __dio_open and __dio_close functions
; Since on the Atari no real open and close is necessary, they
; do not much. The __dio_open queries the sector size of the
; drive which is later used by the _dio_read and _dio_write
; functions.
;
; unsigned char __fastcall__ _dio_open  (_driveid_t drive_id);
; unsigned char __fastcall__ _dio_close (_driveid_t drive_id);
;

 	.export		__dio_open,__dio_close
	.export		sectsizetab
	.include	"atari.inc"

.bss

sectsizetab:
	.res	NUMDRVS * 2

.code


.proc	__dio_open

	asl	a	; make index from drive id
	tax
	lda	#128	; currently hardcoded (until I get an 815 :-)
	sta	sectsizetab,x
	lda	#0
	sta	sectsizetab+1,x

	; fall thru to __dio_close

.endproc

.proc	__dio_close

	lda	#0
	tax
	rts		; return no error

.endproc

