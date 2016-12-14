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
    .include        "telemon30.inc"

.proc   _print
	stx tmp1
	ldy tmp1
	BRK_TELEMON XWSTR0
    rts
.endproc


