;
; Ullrich von Bassewitz, 06.08.1998
;
; CC65 runtime: integer complement
;

        .export         complax

complax:
        eor     #$FF            ; Not A
        pha
        txa
        eor     #$FF            ; Not X
        tax
        pla
        rts

