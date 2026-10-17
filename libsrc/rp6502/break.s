;
; void __fastcall__ set_brk (brk_handler f);
; void reset_brk (void);
;

        .export         _set_brk, _reset_brk
        .export         _brk_a, _brk_x, _brk_y, _brk_sr, _brk_pc

.bss

_brk_a:         .res    1
_brk_x:         .res    1
_brk_y:         .res    1
_brk_sr:        .res    1
_brk_pc:        .res    2
oldvec:         .res    2       ; IRQ/BRK vector before set_brk, 0 if none

.data

uservec:        jmp     $FFFF   ; Patched by set_brk

.code

_set_brk:
        sta     uservec+1
        stx     uservec+2
        lda     oldvec
        ora     oldvec+1
        bne     @set            ; Already installed
        lda     $FFFE
        sta     oldvec
        lda     $FFFF
        sta     oldvec+1
@set:   lda     #<brk_entry
        ldx     #>brk_entry
        bra     setvec

_reset_brk:
        lda     oldvec
        ldx     oldvec+1
        beq     done            ; Not installed
        stz     oldvec
        stz     oldvec+1
setvec: php                     ; An IRQ between the two stores would use
        sei                     ; a half-written vector.
        sta     $FFFE
        stx     $FFFF
        plp
done:   rts

; BRK and IRQ share the vector, so an IRQ goes on to the previous handler.
brk_entry:
        pha
        phx
        tsx
        lda     $0103,x         ; Stacked status register
        and     #$10
        bne     @brk
        plx
        pla
        jmp     (oldvec)

@brk:   pla
        sta     _brk_x
        pla
        sta     _brk_a
        sty     _brk_y
        pla
        and     #$EF            ; Clear break bit
        sta     _brk_sr
        pla                     ; PC low
        sec
        sbc     #2              ; Point to the BRK
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
        rti
