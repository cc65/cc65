;
; Ullrich von Bassewitz, 12.11.2002
;
; time_t __fastcall__ time (time_t* t);
;

	.export		_time

        .import         __errno
        .importzp       ptr1, sreg

        .include        "errno.inc"


.code

.proc   _time

        sta     ptr1
        stx     ptr1+1          ; t

        ldx     #$FF
        stx     sreg
        stx     sreg+1

        ora     ptr1+1          ; t == 0?
        beq     @L1

        ldy     #$03
        txa
@L0:    sta     (ptr1),y        ; *t = -1
        dey
        bpl     @L0
        lda     #$00

@L1:    sta     __errno+1
        lda     #ENOSYS         ; Function not implemented
        sta     __errno

        txa                     ; A = $FF
        rts

.endproc


