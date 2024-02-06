;
; 1998-09-27, Ullrich von Bassewitz
; 2019-11-06, Greg King
;
; void __fastcall__ set_brk (unsigned Addr);
; void reset_brk (void);
;

        .export         _set_brk, _reset_brk
        .destructor     _reset_brk
        .export         _brk_a, _brk_x, _brk_y, _brk_sr, _brk_pc

        .include        "cx16.inc"


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

        lda     BRKVec
        ldx     BRKVec+1
        sta     oldvec
        stx     oldvec+1        ; Save the old vector

L1:     lda     #<brk_handler   ; Set the break vector to our routine
        ldx     #>brk_handler
        sta     BRKVec
        stx     BRKVec+1
        rts

.endproc


; Reset the break vector

.proc   _reset_brk

        lda     oldvec
        ldx     oldvec+1
        beq     @L9             ; Jump if vector not installed
        sta     BRKVec
        stx     BRKVec+1

        lda     #$00
        sta     oldvec          ; Clear the old vector
        sta     oldvec+1
@L9:    rts

.endproc


; Break handler, called if a break occurs

.proc   brk_handler

        pla
        sta     _brk_y
        pla
        sta     _brk_x
        pla
        sta     _brk_a
        pla
        sta     _brk_sr
        pla                     ; PC low
        sec
        sbc     #<$0002         ; Point to start of BRK
        sta     _brk_pc
        pla                     ; PC high
        sbc     #>$0002
        sta     _brk_pc+1

        jsr     uservec         ; Call the user's routine

        lda     _brk_pc+1
        pha
        lda     _brk_pc
        pha
        lda     _brk_sr
        pha
        ldy     _brk_y
        ldx     _brk_x
        lda     _brk_a
        rti                     ; Jump back...

.endproc
