;
; Ullrich von Bassewitz, 24.06.2000
;
; void _maperrno(void);
; /* Map an OS error to a system independent error code */
;

        .include        "errno.inc"


.code

__maperrno:
	lda	__oserror		; Get the error code
	beq	@L1			; Jump if no error
	ldx	#$00			; Clear error
	stx	__oserror
	jsr	__osmaperrno		; Map the code
       	sta    	__errno
	stx	__errno+1
@L1:	rts



