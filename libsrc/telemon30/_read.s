;
; Ullrich von Bassewitz, 2003-04-13
;

;

        .export         _read
        .import         popax
        .importzp       ptr1, ptr2, ptr3, tmp1, tmp2

        .include        "telemon30.inc"
;		int read (int fd, void* buf, unsigned count);

.proc   _read
		;jsr popax ; fp pointer don't care
	sta tmp1 ; count
	stx tmp2 ; count
	jsr popax ; get buf
	;lda #$00
	;ldx #$a0
	sta PTR_READ_DEST
	stx PTR_READ_DEST+1
	lda tmp1 ; 
	ldy tmp2 ;
	BRK_TELEMON XFREAD

	rts

.endproc


