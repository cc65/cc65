; size_t __fastcall__ strnlen (const char* s, size_t maxlen);

.export     _strnlen
.import     popax
.importzp   ptr1, tmp1, tmp2, tmp3, tmp4

.proc _strnlen
        ; Get maximum length.
        tay
        iny
        sty     tmp3
        inx
        stx     tmp4

        ; Fetch string pointer.
        jsr     popax
        sta     ptr1
        stx     ptr1+1

        ; Clear return value.
        ldy     #0
        sty     tmp1
        sty     tmp2


        ;;; Loop over string.
        ; Decrement maximum length.
next:   dec     tmp3
        bne     l2
        dec     tmp4
        beq     done
l2:

        lda     (ptr1),y
        beq     done

        ; Step to next character.
        iny
        bne     l1
        inc     ptr1+1
l1:

        ; Increment return value.
        inc     tmp1
        bne     next
        inc     tmp2

        bne     next        ; (jmp)


done:   lda     tmp1
        ldx     tmp2
        rts
.endproc
