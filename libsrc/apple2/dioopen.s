;
; Oliver Schmidt, 24.03.2005
;
; dhandle_t __fastcall__ dio_open (driveid_t drive_id);
;

        .export 	_dio_open
        .import 	return0, __dos_type

        .include	"errno.inc"
        .include	"mli.inc"

_dio_open:
        ; Check for ProDOS 8
        ldx	__dos_type
        bne	:+
        lda	#$01		; "Bad system call number"
        bne	oserr		; Branch always

        ; Walk device list
:       ldx	DEVCNT
:       cmp	DEVLST,x
        beq	:+		; Found drive_id in device list
        dex
        bpl	:-
        lda	#$28		; "No device detected/connected"
        
        ; Return oserror
oserr:  sta	__oserror
        jmp	return0

        ; Return success
:       ldx	#$00
        stx	__oserror
        rts
