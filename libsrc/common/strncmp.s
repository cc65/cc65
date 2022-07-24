;
; Ullrich von Bassewitz, 25.05.2000
;
; int strncmp (const char* s1, const char* s2, unsigned n);
;

        .export         _strncmp
        .import         popax, popptr1
        .importzp       ptr1, ptr2, ptr3


_strncmp:

; Convert the given counter value in a/x from a downward counter into an
; upward counter, so we can increment the counter in the loop below instead
; of decrementing it. This adds some overhead now, but is cheaper than
; executing a more complex test in each iteration of the loop. We do also
; correct the value by one, so we can do the test on top of the loop.

        eor     #$FF
        sta     ptr3
        txa
        eor     #$FF
        sta     ptr3+1

; Get the remaining arguments

        jsr     popax           ; get s2
        sta     ptr2
        stx     ptr2+1
        jsr     popptr1         ; get s1

; Loop setup

        ;ldy     #0               Y=0 guaranteed by popptr1

; Start of compare loop. Check the counter.

Loop:   inc     ptr3
        beq     IncHi           ; Increment high byte

; Compare a byte from the strings

Comp:   lda     (ptr1),y
        cmp     (ptr2),y
        bne     NotEqual        ; Jump if strings different
        tax                     ; End of strings?
        beq     Equal1          ; Jump if EOS reached, a/x == 0

; Increment the pointers

        iny
        bne     Loop
        inc     ptr1+1
        inc     ptr2+1
        bne     Loop            ; Branch always

; Increment hi byte

IncHi:  inc     ptr3+1
        bne     Comp            ; Jump if counter not zero

; Exit code if strings are equal. a/x not set

Equal:  lda     #$00
        tax
Equal1: rts

; Exit code if strings not equal

NotEqual:
        bcs     L1
        ldx     #$FF            ; Make result negative
        rts

L1:     ldx     #$01            ; Make result positive
        rts


