;
; int __fastcall__ rmdir (const char* name);
;
; UNLINK also removes files, so the directory attribute is tested first.
;

        .export         _rmdir

        .import         __ria_push_path

        .importzp       ptr1

        .include        "rp6502.inc"
        .include        "errno.inc"

_rmdir:
        jsr     __ria_push_path
        bmi     @done
        lda     #RIA_OP_STAT
        sta     RIA_OP
        jsr     RIA_SPIN
        bmi     @done
        ldy     #13             ; fattrib follows fsize and the dates and times
@attr:  lda     RIA_XSTACK
        dey
        bne     @attr
        stz     RIA_OP          ; RIA_OP_DROP_XSTACK
        and     #$10            ; directory
        beq     @notdir
        lda     ptr1
        ldx     ptr1+1
        jsr     __ria_push_path
        lda     #RIA_OP_UNLINK
        sta     RIA_OP
        jmp     RIA_SPIN
@notdir:
        lda     #EINVAL
        jmp     ___directerrno
@done:  rts
