;
; Ullrich von Bassewitz, 2003-12-18
;
; SETNAM kernal call.
;
; NOTE: The routine does not work like that in the CBM610 kernal (which works
; different than that on all other CBMs). Instead, it works like on all other
; Commodore machines. No segment has to be passed, the current segment is
; assumed.

        .export         SETNAM

	.include      	"cbm610.inc"

.proc   SETNAM

        sta     FNAM_LEN
        stx     FNAM_ADR 
        sty     FNAM_ADR+1
        rts

.endproc



