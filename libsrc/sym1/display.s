;
; Wayne Parham (wayne@parhamdata.com)
;
; void fdisp (void);
;

.include        "sym1.inc"

.export         _fdisp

.segment        "CODE"

.proc _fdisp:   near

        jsr     SCAND            ;  Flash Display
        rts

.endproc

