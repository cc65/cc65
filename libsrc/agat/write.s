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
        stx     ptr2+1
        jsr     popptr1
        jsr     popax

        ; Check for zero count
        ora     ptr2
        beq     done

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
        bne     :+
        inc     ptr1+1

        ; Decrement count
:       dec     ptr2
        bne     next
        dec     ptr2+1
        bpl     next

        ; Return success
done:   lda     #$00
        rts

