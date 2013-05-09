;
; Ullrich von Bassewitz, 06.08.1998
;
; void cputsxy (unsigned char x, unsigned char y, char* s);
; void cputs (char* s);
;

        .export         _cputsxy, _cputs
        .import         popa, _gotoxy, _cputc
        .importzp       ptr1, tmp1
                 
_cputsxy:
        sta     ptr1            ; Save s for later
        stx     ptr1+1
        jsr     popa            ; Get Y
        jsr     _gotoxy         ; Set cursor, pop x
        jmp     L0              ; Same as cputs...

_cputs: sta     ptr1            ; Save s
        stx     ptr1+1
L0:     ldy     #0
L1:     lda     (ptr1),y
        beq     L9              ; Jump if done
        iny
        sty     tmp1            ; Save offset
        jsr     _cputc          ; Output char, advance cursor
        ldy     tmp1            ; Get offset
        bne     L1              ; Next char
        inc     ptr1+1          ; Bump high byte
        bne     L1

; Done

L9:     rts


