;
; Oliver Schmidt, 15.01.2004
;
; Error handling for ProDOS 8 file I/O
;

        .export		errnoexit, oserrexit

        .include	"errno.inc"

errnoexit:
        jsr	__seterrno	; Returns with A = 0

oserrexit:
        sta     __oserror
        lda	#$FF
        tax
        rts
