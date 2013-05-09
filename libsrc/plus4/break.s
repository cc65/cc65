;
; Ullrich von Bassewitz, 27.09.1998
;
; void set_brk (unsigned Addr);
; void reset_brk (void);
;

        .export         _set_brk, _reset_brk
        .export         _brk_a, _brk_x, _brk_y, _brk_sr, _brk_pc
        .import         brk_jmp

        .include        "plus4.inc"


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

        lda     #<brk_handler   ; Set the break vector to our routine
        sta     brk_jmp+1
        lda     #>brk_handler
        sta     brk_jmp+2
        rts

.endproc


; Reset the break vector
.proc   _reset_brk

        lda     #$00
        sta     brk_jmp+1
        sta     brk_jmp+2       ; Reset the vector
        rts

.endproc



; Break handler, called if a break occurs. 

.proc   brk_handler

        pla
        sta     _brk_y
        pla
        sta     _brk_x
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


