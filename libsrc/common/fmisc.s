;
; Ullrich von Bassewitz, 31.05.1998
;
; Several small file stream functions
;

	.export		_clearerr, _feof, _ferror, _fileno, _fflush
	.import	 	return0
	.importzp 	ptr1

        .include        "_file.inc"

;
; Get the FILE* parameter, check if the file is open
;

getf:	sta	ptr1
	stx	ptr1+1
	ldy	#_FILE::f_flags
	lda	(ptr1),y      	; get f->f_flags
	and	#_FOPEN         ; file open?
	beq    	@L1   		; jump if no
	clc	      		; ok
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
       	and	#<~(_FEOF | _FERROR)
       	sta	(ptr1),y
err:	rts

;
; int feof (FILE* f);
;

_feof:
 	jsr	getf
;	bcs	err
 	lda	(ptr1),y
 	and	#_FEOF
 	ldx	#0
 	rts

;
; int ferror (FILE* f);
;

_ferror:
 	jsr	getf
;	bcs	err
 	lda	(ptr1),y
 	and	#_FERROR
 	ldx	#0
 	rts

;
; int fileno (FILE* f);
;

_fileno:
 	jsr	getf
; 	bcs	err
	ldy	#_FILE::f_fd
 	lda	(ptr1),y
 	ldx	#0
 	rts

;
; int __fastcall__ fflush (FILE* f);
;

_fflush	= return0


