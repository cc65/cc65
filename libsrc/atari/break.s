;
; Christian Groessler, 27-Feb-2000
;
; void set_brk (unsigned Addr);
; void reset_brk (void);
;

        .export         _set_brk, _reset_brk
        .destructor     _reset_brk
        .export         _brk_a, _brk_x, _brk_y, _brk_sr, _brk_pc

        .include        "atari.inc"


.bss
_brk_a:         .res    1
_brk_x:         .res    1
_brk_y:         .res    1
_brk_sr:        .res    1
_brk_pc:        .res    2

oldvec:         .res    2               ; Old vector


.data
uservec:        jmp     $FFFF           ; Patched at runtime


.code

; Set the break vector
.proc   _set_brk

        sta     uservec+1
        stx     uservec+2       ; Set the user vector

        lda     oldvec
        ora     oldvec+1        ; Did we save the vector already?
        bne     L1              ; Jump if we installed the handler already

        lda     VBREAK
        sta     oldvec
        lda     VBREAK+1
        sta     oldvec+1        ; Save the old vector

L1:     lda     #<brk_handler   ; Set the break vector to our routine
        sta     VBREAK
        lda     #>brk_handler
        sta     VBREAK+1
        rts

.endproc


; Reset the break vector
.proc   _reset_brk

        lda     oldvec
        ldx     oldvec+1
        beq     @L9             ; Jump if vector not installed
        sta     VBREAK
        stx     VBREAK+1
        lda     #$00
        sta     oldvec          ; Clear the old vector
        stx     oldvec+1
@L9:    rts     

.endproc



; Break handler, called if a break occurs

.proc   brk_handler

        sty     _brk_y
        stx     _brk_x
        pla
        sta     _brk_a
        pla
        and     #$EF            ; Clear break bit
        sta     _brk_sr
        pla                     ; PC low
        sec
        sbc     #2              ; Point to start of brk
        sta     _brk_pc
        pla                     ; PC high
        sbc     #0
        sta     _brk_pc+1

        jsr     uservec         ; Call the user's routine

        lda     _brk_pc+1
        pha
        lda     _brk_pc
        pha
        lda     _brk_sr
        pha
        ldx     _brk_x
        ldy     _brk_y
        lda     _brk_a
        rti                     ; Jump back...

.endproc


