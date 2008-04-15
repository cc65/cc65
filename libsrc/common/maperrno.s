;
; Ullrich von Bassewitz, 24.06.2000
;
; void _maperrno(void);
; /* Map an OS error to a system independent error code */
;
; Second entry setoserror maps the OS error code in A to an errno code
; and stores it into errno. _oserror is cleared.


        .include        "errno.inc"


.code

__maperrno:
	lda	__oserror		; Get the error code
       	beq    	L9			; Jump if no error
setoserror:
	ldx	#$00			; Clear error
	stx	__oserror
	jsr	__osmaperrno		; Map the code
       	sta    	__errno
	stx	__errno+1
L9:	rts       



