;
; time_t __fastcall__ mktime (struct tm* timep);
;

        .export         _mktime

        .import         __ria_call_time, __ria_time_fail
        .import         _ria_call_int, _ria_push_long

        .importzp       ptr1, ptr2, sreg, tmp1

        .include        "rp6502.inc"

_mktime:
        sta     ptr1
        stx     ptr1+1          ; Save timep
        ora     ptr1+1
        bne     @push
        jmp     __ria_time_fail ; NULL returns -1 without an OS call
@push:  ldy     #17             ; sizeof(struct tm)-1, pushed in reverse
@loop:  lda     (ptr1),y
        sta     RIA_XSTACK
        dey
        bpl     @loop
        lda     #RIA_OP_MKTIME
        jsr     __ria_call_time ; A:X:sreg = time_t (or -1)
        bcs     @ret            ; error/overflow -> return -1, errno set
        sta     tmp1
        stx     ptr2            ; X survives in ptr2 across the call
        jsr     _ria_push_long  ; normalized write-back via LOCALTIME
        lda     #RIA_OP_LOCALTIME
        jsr     _ria_call_int   ; N = sign of int result
        bmi     @load
        ldy     #0
@pop:   lda     RIA_XSTACK
        sta     (ptr1),y
        iny
        cpy     #18             ; sizeof(struct tm)
        bne     @pop
@load:  lda     tmp1
        ldx     ptr2
@ret:   rts
