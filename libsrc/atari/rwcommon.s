;
; common iocb setup routine for read, write
; expects parameters (int fd,void *buf,int count)
;
        .include "atari.inc"
	.include "../common/errno.inc"
        .importzp tmp2,tmp3
        .import incsp6,ldax0sp,ldaxysp
	.import __errno,__oserror
	.import	fdtoiocb
        
        .export __rwsetup

__rwsetup:
	
        ldy     #5
        jsr     ldaxysp         ; get fd
	jsr	fdtoiocb	; convert to iocb
	bmi	iocberr
        sta     tmp3            ; save it
        jsr     ldax0sp         ; get size
        stx     tmp2
        ldx     tmp3            ; iocb
	cpx	#$80		; iocb must be 0...7
	bcs	iocberr
        sta     ICBLL,x
        lda     tmp2            ; size hi
        sta     ICBLH,x
        ldy     #3              ; get buf addr (was 2 in orig. version)
        jsr     ldaxysp
        stx     tmp2
        ldx     tmp3
        sta     ICBAL,x
        lda     tmp2
        sta     ICBAH,x
	jsr     incsp6          ; pop args
	lda	ICBLL,x
	ora	ICBLH,x		; return with Z if length was 0
        rts

iocberr:jsr     incsp6          ; pop args
	ldx	#$FF		; indicate error + clear ZF
	rts

	
;
; this routine updates errno.  do a JMP here right after calling
; CIOV.  we expect status in Y.
;
        .export __do_oserror,__seterrno,__inviocb
__do_oserror:
	sty	__oserror	; save os dependent error code
retminus:	
	lda	#$FF
	tax			; return -1
	rts

__seterrno:
        sta     __errno
        stx     __errno+1
        rts

;
; sets EINVAL error code and returns -1
;

__inviocb:
	lda	#<EINVAL
	ldx	#>EINVAL
	jsr	__seterrno
	jmp	retminus	; return -1
