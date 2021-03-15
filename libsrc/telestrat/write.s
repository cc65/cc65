;
; jede jede@oric.org 2017-01-22

        .export         _write
        .import         popax, popptr1
        .importzp       ptr1, ptr2, ptr3, tmp1

        .include        "telestrat.inc"

; int write (int fd, const void* buf, int count);
.proc   _write

        sta     ptr3
        stx     ptr3+1          ; Save count as result

        inx
        stx     ptr2+1
        tax
        inx
        stx     ptr2            ; Save count with each byte incremented separately

        jsr     popptr1         ; Get buf
        jsr     popax           ; Get fd and discard

        ; If fd=0001 then it stdout
        cpx     #0
        beq     next
        jmp     L1
next:
        cmp     #1
        beq     L1

        ; Here it's a file opened
        lda     ptr1
        sta     PTR_READ_DEST
        lda     ptr1+1
        sta     PTR_READ_DEST+1
        lda     ptr3
        ldy     ptr3+1
        BRK_TELEMON  XFWRITE
        ;  Compute nb of bytes written


        lda     PTR_READ_DEST+1
        sec
        sbc     ptr1+1
        tax
        lda     PTR_READ_DEST
        sec
        sbc     ptr1
        rts


L1:     dec     ptr2
        bne     L2
        dec     ptr2+1
        beq     L9
L2:     ldy     #0
        lda     (ptr1),y
        tax
        cpx     #$0A            ; Check for \n
        bne     L3
        BRK_TELEMON  XWR0       ; Macro send char to screen (channel 0 in telemon terms)
        lda     #$0D            ; Return to the beggining of the line
        BRK_TELEMON  XWR0       ; Macro


        ldx     #$0D
L3:
        BRK_TELEMON  XWR0       ; Macro

        inc     ptr1
        bne     L1
        inc     ptr1+1
        jmp     L1

        ; No error, return count

L9:     lda     ptr3
        ldx     ptr3+1
        rts

.endproc
