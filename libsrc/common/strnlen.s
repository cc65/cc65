; size_t __fastcall__ strnlen (const char* s, size_t maxlen);

.export     _strnlen
.import     popax
.importzp   ptr1, tmp1, tmp2, tmp3, tmp4

.proc _strnlen
        ; Fetch string pointer.
        sta     ptr1
        stx     ptr1+1

        ; Clear return value.
        lda     #0
        sta     tmp1
        sta     tmp2

        ; Get maximum length.
        jsr     popax
        sta     tmp3
        inc     tmp3
        inx
        stx     tmp4

        ;;; Loop over string.
        ldy     #0

        ; Decrement maximum length.
next:   dec     tmp3
        bne     l2
        dec     tmp4
        beq     done
l2:

        lda     (ptr1),y
        beq     done

        ; Step to next character.
        inc     ptr1
        bne     l1
        inc     ptr1+1
l1:

        ; Increment return value.
        inc     tmp1
        bne     next
        inc     tmp2

        jmp     next


done:   lda     tmp1
        ldx     tmp2
        rts
.endproc
