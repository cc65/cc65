;
; Ullrich von Bassewitz, 2003-12-18
;
; UPDST kernal call.
;

        .export         UPDST

	.include      	"cbm610.inc"

.proc   UPDST

        ora     ST
        sta     ST
        rts

.endproc



