;
; Wayne Parham (wayne@parhamdata.com)
;
; void fdisp (void);
;

.include        "sym1.inc"

.export         _fdisp

.segment        "CODE"

.proc _fdisp:   near

        jmp     SCAND            ;  Flash Display

.endproc
