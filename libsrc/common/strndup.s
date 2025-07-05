;
; Colin Leroy-Mira, 03.07.2025
;
; char* __fastcall__ strndup (const char* S, size_t maxlen);
;

        .importzp       tmp1, tmp2, ptr2
        .import         _strncpy, _strlen, _malloc
        .import         pushax, popax, incsp2, incax1, swapstk
        .import         ___errno

        .export         _strndup

        .include        "errno.inc"

.proc _strndup
        sta     tmp1          ; Remember maxlen
        stx     tmp1+1

        jsr     popax         ; Get string
        jsr     pushax        ; Keep it in TOS

        jsr     _strlen       ; Get string length,
        cpx     tmp1+1        ; Compare to max,
        bcc     alloc
        bne     :+
        cmp     tmp1
        bcc     alloc

:       lda     tmp1          ; Use maxlen if shorter
        ldx     tmp1+1

alloc:  jsr     incax1        ; Add 1 for terminator
        jsr     _malloc       ; Allocate output
        cpx     #$00          ; Check allocation
        beq     errmem

        jsr     swapstk       ; Put dest in TOS and get string back
        jsr     pushax        ; Put src in TOS
        lda     tmp1          ; Get length for strncpy
        ldx     tmp1+1

        jsr     _strncpy      ; Copy
        pha                   ; Terminate
        lda     #$00
        sta     (ptr2),y
        pla
        rts

errmem: ldy     #ENOMEM
        sty     ___errno
        jmp     incsp2        ; Pop string and return
.endproc
