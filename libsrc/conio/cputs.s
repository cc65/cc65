;
; Ullrich von Bassewitz, 06.08.1998
;
; void cputsxy (unsigned char x, unsigned char y, const char* s);
; void cputs (const char* s);
;

        .export         _cputsxy, _cputs
        .import         gotoxy, _cputc
        .importzp       ptr1, tmp1
        .macpack        cpu

_cputsxy:
        sta     ptr1            ; Save s for later
        stx     ptr1+1
        jsr     gotoxy          ; Set cursor, pop x and y
.if (.cpu .bitand CPU_ISET_65SC02)
        bra     L0              ; Same as cputs...
.else
        jmp     L0              ; Same as cputs...
.endif

_cputs: sta     ptr1            ; Save s
        stx     ptr1+1

.if (.cpu .bitand CPU_ISET_65SC02)

L0:     lda     (ptr1)          ; (5)
        beq     L9              ; (7)  Jump if done
        jsr     _cputc          ; (13) Output char, advance cursor
        inc     ptr1            ; (18) Bump low byte
        bne     L0              ; (20) Next char
        inc     ptr1+1          ; (25) Bump high byte
        bne     L0

.else

L0:     ldy     #0              ; (2)
L1:     lda     (ptr1),y        ; (7)
        beq     L9              ; (9)  Jump if done
        iny
        sty     tmp1            ; (14) Save offset
        jsr     _cputc          ; (20) Output char, advance cursor
        ldy     tmp1            ; (23) Get offset
        bne     L1              ; (25) Next char
        inc     ptr1+1          ; (30) Bump high byte
        bne     L1

.endif

; Done

L9:     rts
