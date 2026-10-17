;
; time_t __fastcall__ mktime (struct tm* timep);
;

        .export         _mktime

        .import         __ria_call_time

        .importzp       ptr1, sreg, tmp1

        .include        "rp6502.inc"

_mktime:
        sta     ptr1
        stx     ptr1+1          ; Save timep
        ldy     #17             ; sizeof(struct tm)-1, pushed in reverse
@loop:  lda     (ptr1),y
        sta     RIA_XSTACK
        dey
        bpl     @loop
        lda     #RIA_OP_MKTIME
        jsr     __ria_call_time ; A:X:sreg = time_t (or -1)
        bcs     @ret            ; error/overflow -> return -1, errno set
        phx                     ; X survives on the stack across the call
        stz     RIA_XSTACK      ; zero on top keeps the unsigned time_t positive
        ldy     sreg+1          ; normalized write-back via LOCALTIME
        sty     RIA_XSTACK
        ldy     sreg
        sty     RIA_XSTACK
        stx     RIA_XSTACK
        sta     RIA_XSTACK
        lda     #RIA_OP_LOCALTIME
        sta     RIA_OP
        jsr     RIA_SPIN        ; N = sign of int result
        bmi     @load
        ldy     #0
@pop:   lda     RIA_XSTACK
        sta     (ptr1),y
        iny
        cpy     #18             ; sizeof(struct tm)
        bne     @pop
@load:  lda     tmp1
        plx
@ret:   rts
