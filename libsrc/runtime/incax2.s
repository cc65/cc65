;
; Ullrich von Bassewitz, 05.08.1998
;
; CC65 runtime: Increment ax by 2
;

        .export         incax2

        .macpack        generic

.proc   incax2

        add     #2
        bcc     @L9
        inx
@L9:    rts

.endproc
