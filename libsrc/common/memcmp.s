;
; Ullrich von Bassewitz, 15.09.2000
;
; int memcmp (const void* p1, const void* p2, size_t count);
;

        .export         _memcmp
        .import         popax, popptr1, return0
        .importzp       ptr1, ptr2, ptr3

_memcmp:

; Calculate a special count, and store it into ptr3. That is some overhead here,
; but saves time in the compare loop

        inx
        stx     ptr3+1
        tax
        inx
        stx     ptr3            ; Save count with each byte incremented separately

; Get the pointer parameters

        jsr     popax           ; Get p2
        sta     ptr2
        stx     ptr2+1
        jsr     popptr1         ; Get p1

; Loop initialization

        ;ldy     #$00           ; Initialize pointer (Y=0 guaranteed by popptr1)
        ldx     ptr3            ; Load inner counter byte into .X

; Head of compare loop: Test for the end condition

Loop:   dex
        beq     BumpHiCnt       ; Jump on end of inner count

; Do the compare

Comp:   lda     (ptr1),y
        cmp     (ptr2),y
        bne     NotEqual        ; Jump if bytes not equal

; Bump the pointers

        iny                     ; Increment pointer
        bne     Loop
        inc     ptr1+1          ; Increment high bytes
        inc     ptr2+1
        bne     Loop            ; Branch always (pointer wrap is illegal)

; Entry on inner loop end

BumpHiCnt:
        dec     ptr3+1
        bne     Comp            ; Jump if not done
        jmp     return0         ; Count is zero, areas are identical

; Not equal, check which one is greater

NotEqual:
        bcs     Greater
        ldx     #$FF            ; Make result negative
        rts

Greater:
        ldx     #$01            ; Make result positive
        rts
