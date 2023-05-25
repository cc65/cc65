;
; jede jede@oric.org 2017-01-22

        .export         _write
        .import         popax, popptr1

        .include        "zeropage.inc"
        .include        "telestrat.inc"

; int write (int fd, const void* buf, int count);
.proc   _write

        sta     ptr3
        stx     ptr3+1          ; save count as result

        inx
        stx     ptr2+1
        tax
        inx
        stx     ptr2            ; save count with each byte incremented separately

        jsr     popptr1         ; get buf

        jsr     popax           ; get fd

        sta     tmp1            ; save fd

        ; if fd=0001 then it stdout
        cpx     #0
        beq     next
        jmp     L1
next:
        cmp     #1
        beq     L1

        ; here it's a file opened
        lda     ptr1
        sta     PTR_READ_DEST
        lda     ptr1+1
        sta     PTR_READ_DEST+1
        lda     ptr3
        ldy     ptr3+1
        ldx     tmp1            ; send fd in X
        BRK_TELEMON  XFWRITE

        ;  compute nb of bytes written
        sec
        lda     PTR_READ_DEST
        sbc     ptr1
        pha
        lda     PTR_READ_DEST+1
        sbc     ptr1+1
        tax
        pla
        rts


L1:     dec     ptr2
        bne     L2
        dec     ptr2+1
        beq     L9
L2:     ldy     #0
        lda     (ptr1),y
        tax
        cpx     #$0A            ; check for \n
        bne     L3
        BRK_TELEMON  XWR0       ; macro send char to screen (channel 0 in telemon terms)
        lda     #$0D            ; return to the beggining of the line
        BRK_TELEMON  XWR0       ; macro


        ldx     #$0D
L3:
        BRK_TELEMON  XWR0       ; macro

        inc     ptr1
        bne     L1
        inc     ptr1+1
        jmp     L1

        ; no error, return count

L9:     lda     ptr3
        ldx     ptr3+1
        rts

.endproc
