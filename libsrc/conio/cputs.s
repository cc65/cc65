;
; Ullrich von Bassewitz, 06.08.1998
;
; void cputsxy (unsigned char x, unsigned char y, const char* s);
; void cputs (const char* s);
;

        .export         _cputsxy, _cputs
        .import         gotoxy, _cputc
        .importzp       ptr1, tmp1

_cputsxy:
        sta     ptr1            ; Save s for later
        stx     ptr1+1
        jsr     gotoxy          ; Set cursor, pop x and y
.if .cap(CPU_HAS_BRA8)
        bra     L0              ; Same as cputs...
.else
        jmp     L0              ; Same as cputs...
.endif

_cputs: sta     ptr1            ; Save s
        stx     ptr1+1

; Important note: The implementation below relies on the _cputc() function not
; clobbering ptr1. This might not be the case when rewriting this function so
; beware!

L0:
.if .cap(CPU_HAS_ZPIND)
        lda     (ptr1)          ; (5)
.else
        ldy     #0              ; (2)
        lda     (ptr1),y        ; (7)
.endif
        beq     L9              ; (7/9)  Jump if done
        jsr     _cputc          ; (13/15) Output char, advance cursor
        inc     ptr1            ; (18/20) Bump low byte
        bne     L0              ; (20/22) Next char
        inc     ptr1+1          ; (25/27) Bump high byte
        bne     L0
L9:     rts
