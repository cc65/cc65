;
; Oleg A. Odintsov, Moscow, 2024
;
; int __fastcall__ write (int fd, const void* buf, unsigned count);
;

        .export         _write
        .import         popax, popptr1
        .import         COUT

        .include        "zeropage.inc"

_write:
        sta     ptr2
        sta     ptr3
        stx     ptr2+1
        stx     ptr3+1
        jsr     popptr1
        jsr     popax

        ; Fix count to ease counting down
        inc     ptr2
        inc     ptr2+1
        bne     check_loop      ; branch always - check if we have anything to write

        ; Get char from buf
next:   ldy     #$00
        lda     (ptr1),y

        ; Replace '\n' with '\r'
        cmp     #$0A
        bne     output
        lda     #$8D

        ; Set hi bit and write to device
output:
        jsr     COUT            ; Preserves X and Y

        ; Increment pointer
        inc     ptr1
        bne     check_loop
        inc     ptr1+1

        ; Decrement count
check_loop:
        dec     ptr2
        bne     next
        dec     ptr2+1
        bne     next

        ; Return success
done:   lda     ptr3
        ldx     ptr3+1
        rts
