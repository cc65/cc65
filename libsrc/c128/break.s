;
; Ullrich von Bassewitz, 27.09.1998
;
; void set_brk (unsigned Addr);
; void reset_brk (void);
;

        .export         _set_brk, _reset_brk
        .destructor     _reset_brk
        .export         _brk_a, _brk_x, _brk_y, _brk_sr, _brk_pc
        .importzp       ptr1

        .include        "c128.inc"


.bss
_brk_a:         .res    1
_brk_x:         .res    1
_brk_y:         .res    1
_brk_sr:        .res    1
_brk_pc:        .res    2

.data
uservec:        jmp     $FFFF           ; Patched at runtime

.code


; Set the break vector
.proc   _set_brk

        sta     uservec+1
        stx     uservec+2       ; Set the user vector

        lda     brk_old+1
        ora     brk_old+2       ; Did we save the vector already?
        bne     @L1             ; Jump if we installed the handler already

        lda     BRKVec          ; Save the old vector
        sta     brk_old+1
        lda     BRKVec+1
        sta     brk_old+2

        lda     #<brk_stub      ; Set the break vector to our stub
        ldx     #>brk_stub
        sta     BRKVec
        stx     BRKVec+1

        lda     #<brk_handler   ; Set the indirect vector to our handler
        ldx     #>brk_handler
        sta     brk_ind+1
        stx     brk_ind+2

@L1:    rts

.endproc


; Reset the break vector
.proc   _reset_brk

        lda     brk_old+1
        ldx     brk_old+2
        beq     @L9             ; Jump if vector not installed
        sta     BRKVec
        stx     BRKVec+1
        lda     #$00
        sta     brk_old+1       ; Clear the saved vector
        sta     brk_old+2
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


; Break stub, must go into low (non banked) memory

.segment        "LOWCODE"

.proc   brk_stub
        pla                             ; Get original MMU_CR value
        sta     MMU_CR                  ; And set it
        jmp     brk_ind                 ; Jump indirect to break
.endproc

; ------------------------------------------------------------------------
; Data

.data

; Old break vector preceeded by a jump opcode
brk_old:
        jmp     $0000

; Indirect vectors preceeded by a jump opcode
brk_ind:
        jmp     $0000

