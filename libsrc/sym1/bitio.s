; ---------------------------------------------------------------------------
; bitio.s
;
; for Sym-1
;
; Wayne Parham
;
; wayne@parhamdata.com
; ---------------------------------------------------------------------------

.include        "sym1.inc"

.export         _set_DDR1A, _get_DDR1A, _set_IOR1A, _get_IOR1A
.export         _set_DDR1B, _get_DDR1B, _set_IOR1B, _get_IOR1B
.export         _set_DDR2A, _get_DDR2A, _set_IOR2A, _get_IOR2A
.export         _set_DDR2B, _get_DDR2B, _set_IOR2B, _get_IOR2B
.export         _set_DDR3A, _get_DDR3A, _set_IOR3A, _get_IOR3A
.export         _set_DDR3B, _get_DDR3B, _set_IOR3B, _get_IOR3B

.segment        "CODE"

.proc _set_DDR1A: near
; ---------------------------------------------------------------------------
        sta     DDR1A            ;  Write data direction register for port 1A
        lda     #$00             ;
        ldx     #$00             ;
        rts                      ;  Return 0000
; ---------------------------------------------------------------------------
.endproc

.proc _get_DDR1A: near
; ---------------------------------------------------------------------------
        lda     DDR1A            ;  Read data direction register for port 1A
        ldx     #$00             ;
        rts                      ;  Return DDR1A
; ---------------------------------------------------------------------------
.endproc

.proc _set_IOR1A: near
; ---------------------------------------------------------------------------
        sta     OR1A             ;  Write I/O register for port 1A
        lda     #$00             ;
        ldx     #$00             ;
        rts                      ;  Return 0000
; ---------------------------------------------------------------------------
.endproc

.proc _get_IOR1A: near
; ---------------------------------------------------------------------------
        lda     OR1A             ;  Read I/O register for port 1A
        ldx     #$00             ;
        rts                      ;  Return OR1A
; ---------------------------------------------------------------------------
.endproc


.proc _set_DDR1B: near
; ---------------------------------------------------------------------------
        sta     DDR1B            ;  Write data direction register for port 1B
        lda     #$00             ;
        ldx     #$00             ;
        rts                      ;  Return 0000
; ---------------------------------------------------------------------------
.endproc

.proc _get_DDR1B: near
; ---------------------------------------------------------------------------
        lda     DDR1B            ;  Read data direction register for port 1B
        ldx     #$00             ;
        rts                      ;  Return DDR1B
; ---------------------------------------------------------------------------
.endproc

.proc _set_IOR1B: near
; ---------------------------------------------------------------------------
        sta     OR1B             ;  Write I/O register for port 1B
        lda     #$00             ;
        ldx     #$00             ;
        rts                      ;  Return 0000
; ---------------------------------------------------------------------------
.endproc

.proc _get_IOR1B: near
; ---------------------------------------------------------------------------
        lda     OR1B             ;  Read I/O register for port 1B
        ldx     #$00             ;
        rts                      ;  Return OR1B
; ---------------------------------------------------------------------------
.endproc


.proc _set_DDR2A: near
; ---------------------------------------------------------------------------
        sta     DDR2A            ;  Write data direction register for port 2A
        lda     #$00             ;
        ldx     #$00             ;
        rts                      ;  Return 0000
; ---------------------------------------------------------------------------
.endproc

.proc _get_DDR2A: near
; ---------------------------------------------------------------------------
        lda     DDR2A            ;  Read data direction register for port 2A
        ldx     #$00             ;
        rts                      ;  Return DDR2A
; ---------------------------------------------------------------------------
.endproc

.proc _set_IOR2A: near
; ---------------------------------------------------------------------------
        sta     OR2A             ;  Write I/O register for port 2A
        lda     #$00             ;
        ldx     #$00             ;
        rts                      ;  Return 0000
; ---------------------------------------------------------------------------
.endproc

.proc _get_IOR2A: near
; ---------------------------------------------------------------------------
        lda     OR2A             ;  Read I/O register for port 2A
        ldx     #$00             ;
        rts                      ;  Return OR2A
; ---------------------------------------------------------------------------
.endproc


.proc _set_DDR2B: near
; ---------------------------------------------------------------------------
        sta     DDR2B            ;  Write data direction register for port 2B
        lda     #$00             ;
        ldx     #$00             ;
        rts                      ;  Return 0000
; ---------------------------------------------------------------------------
.endproc

.proc _get_DDR2B: near
; ---------------------------------------------------------------------------
        lda     DDR2B            ;  Read data direction register for port 2B
        ldx     #$00             ;
        rts                      ;  Return DDR2B
; ---------------------------------------------------------------------------
.endproc

.proc _set_IOR2B: near
; ---------------------------------------------------------------------------
        sta     OR2B             ;  Write I/O register for port 2B
        lda     #$00             ;
        ldx     #$00             ;
        rts                      ;  Return 0000
; ---------------------------------------------------------------------------
.endproc

.proc _get_IOR2B: near
; ---------------------------------------------------------------------------
        lda     OR2B             ;  Read I/O register for port 2B
        ldx     #$00             ;
        rts                      ;  Return OR2B
; ---------------------------------------------------------------------------
.endproc


.proc _set_DDR3A: near
; ---------------------------------------------------------------------------
        sta     DDR3A            ;  Write data direction register for port 3A
        lda     #$00             ;
        ldx     #$00             ;
        rts                      ;  Return 0000
; ---------------------------------------------------------------------------
.endproc

.proc _get_DDR3A: near
; ---------------------------------------------------------------------------
        lda     DDR3A            ;  Read data direction register for port 3A
        ldx     #$00             ;
        rts                      ;  Return DDR3A
; ---------------------------------------------------------------------------
.endproc

.proc _set_IOR3A: near
; ---------------------------------------------------------------------------
        sta     OR3A             ;  Write I/O register for port 3A
        lda     #$00             ;
        ldx     #$00             ;
        rts                      ;  Return 0000
; ---------------------------------------------------------------------------
.endproc

.proc _get_IOR3A: near
; ---------------------------------------------------------------------------
        lda     OR3A             ;  Read I/O register for port 3A
        ldx     #$00             ;
        rts                      ;  Return OR3A
; ---------------------------------------------------------------------------
.endproc


.proc _set_DDR3B: near
; ---------------------------------------------------------------------------
        sta     DDR3B            ;  Write data direction register for port 3B
        lda     #$00             ;
        ldx     #$00             ;
        rts                      ;  Return 0000
; ---------------------------------------------------------------------------
.endproc

.proc _get_DDR3B: near
; ---------------------------------------------------------------------------
        lda     DDR3B            ;  Read data direction register for port 3B
        ldx     #$00             ;
        rts                      ;  Return DDR3B
; ---------------------------------------------------------------------------
.endproc

.proc _set_IOR3B: near
; ---------------------------------------------------------------------------
        sta     OR3B             ;  Write I/O register for port 3B
        lda     #$00             ;
        ldx     #$00             ;
        rts                      ;  Return 0000
; ---------------------------------------------------------------------------
.endproc

.proc _get_IOR3B: near
; ---------------------------------------------------------------------------
        lda     OR3B             ;  Read I/O register for port 3B
        ldx     #$00             ;
        rts                      ;  Return OR3B
; ---------------------------------------------------------------------------
.endproc

