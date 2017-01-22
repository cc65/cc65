;
; Jede
;
; print (char * str);
;
; This function is a hack!
;

        .export         _print
        .import         popax
	.importzp       tmp1
        .include        "telemon24.inc"

.proc   _print

        jsr     popax           ; get buf
	stx tmp1
	ldy tmp1
	brk
	.byte $14
        rts

.endproc


