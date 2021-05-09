; ---------------------------------------------------------------------------
; putchar.s
;
; for Sym-1
;
; Wayne Parham
;
; wayne@parhamdata.com
; ---------------------------------------------------------------------------

.include        "sym1.inc"
.export         _putchar

.segment        "CODE"
   
.proc _putchar: near
; ---------------------------------------------------------------------------
        jsr     OUTCHR           ;  Send character using Monitor ROM call
        lda     #$00             ;
        ldx     #$00             ;
        rts                      ;  Return 0000
; ---------------------------------------------------------------------------
.endproc
