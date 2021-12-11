;
; Wayne Parham (wayne@parhamdata.com)
;
; void beep (void);
;

.include        "sym1.inc"

.export         _beep

.segment        "CODE"

.proc _beep:    near

        jsr     BEEP             ;  Beep
        rts

.endproc
