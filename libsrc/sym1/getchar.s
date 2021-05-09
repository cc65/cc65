; ---------------------------------------------------------------------------
; getchar.s
;
; for Sym-1
;
; Wayne Parham
; ---------------------------------------------------------------------------

.include        "sym1.inc"
.export         _getchar

.segment        "CODE"

.proc _getchar: near
; ---------------------------------------------------------------------------
        jsr     INTCHR           ;  Get character using Monitor ROM call
        and     #$7F             ;  Strip off top bit
        ldx     #$00             ;
        rts                      ;  Return char
; ---------------------------------------------------------------------------
.endproc
