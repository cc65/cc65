;
; Oliver Schmidt, 24.03.2005
;
; dhandle_t __fastcall__ dio_open (driveid_t drive_id);
;
; drive_id = (slot * 2) + (drive - 1)

        .export 	_dio_open
        .import		return0

        .include	"zeropage.inc"
        .include	"errno.inc"
        .include	"mli.inc"

_dio_open:
        ; Convert drive id into unit number
        lsr
        bcc	:+
        ora	#%00001000
:       asl
        asl
        asl
        asl
        tay			; Save handle

        ; Set handle
        sta	mliparam + MLI::ON_LINE::UNIT_NUM

        ; Alloc 16-byte buffer just below stack
        lda	sp
        sec
        sbc	#16
        sta	mliparam + MLI::ON_LINE::DATA_BUFFER
        lda	sp+1
        sbc	#$00
        sta	mliparam + MLI::ON_LINE::DATA_BUFFER+1

        ; Get device state
        lda	#ON_LINE_CALL
        ldx	#ON_LINE_COUNT
        jsr	callmli
        bcc	:+

        ; DIO level access doesn't necessarily need a
        ; ProDOS 8 disk so ignore "high level" errors
        cmp	#$40
        bcc	oserr

        ; Return success
:       tya			; Restore handle
        ldx	#$00
        stx	__oserror
        rts

        ; Return oserror
oserr:  sta     __oserror
        jmp	return0
