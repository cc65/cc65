;
; Oliver Schmidt, 30.12.2004
;
; int __fastcall__ close (int fd);
;

        .export 	_close

        .import		closedirect, freebuffer
        .import 	errnoexit, oserrexit
        .import 	return0

        .include	"filedes.inc"

_close:
        ; Process fd
        jsr	getfd		; Returns A, Y and C
        bcs	errno

        ; Check for device
        bmi	zerofd

        ; Close file
        jsr	closedirect	; Preserves Y
        bcs	oserr

        ; Mark fdtab slot as free
zerofd: lda	#$00
        sta	fdtab + FD::REF_NUM,y

        ; Cleanup I/O buffer
        jsr	freebuffer

        ; Return success
        jmp	return0

        ; Return errno
errno:  jmp	errnoexit

        ; Return oserror
oserr:  jmp	oserrexit
