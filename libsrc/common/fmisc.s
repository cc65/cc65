;
; Ullrich von Bassewitz, 31.05.1998
;
; Several small file stream functions
;

	.export		_clearerr, _feof, _ferror, _fileno, _fflush
	.import	 	return0
	.import	 	__errno
	.importzp 	ptr1

;
; Get the FILE* parameter, check if the file is open
;

getf:	sta	ptr1
	stx	ptr1+1
	ldy	#1
	lda	(ptr1),y	; get f->f_flags
	and	#$01 		; file open?
	beq    	@L1		; jump if no
	clc	     		; ok
	rts
@L1:	sec
	rts

;
; void clearerr (FILE* f);
;

_clearerr:
       	jsr	getf
       	bcs	err
       	lda	(ptr1),y
       	and	#$F9
       	sta	(ptr1),y
err:	rts

;
; int feof (FILE* f);
;

_feof:
 	jsr	getf
;	bcs	err
 	lda	(ptr1),y
 	and	#$02
 	ldx	#0
 	rts

;
; int ferror (FILE* f);
;

_ferror:
 	jsr	getf
;	bcs	err
 	lda	(ptr1),y
 	and	#$04
 	ldx	#0
 	rts

;
; int fileno (FILE* f);
;

_fileno:
 	jsr	getf
; 	bcs	err
 	dey
 	lda	(ptr1),y
 	ldx	#0
 	rts

;
; int __fastcall__ fflush (FILE* f);
;

_fflush	= return0


