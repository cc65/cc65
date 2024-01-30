;
; Colin Leroy-Mira, 2024
;
; int __fastcall__ fputs (const char* s, register FILE* f)
;

        .export         _fputs
        .importzp       ptr1, ptr2
        .import         _write, _strlen, checkferror
        .import         swapstk, pushax, returnFFFF

        .include        "stdio.inc"
        .include        "_file.inc"

_fputs:
        sta     ptr1
        stx     ptr1+1

        jsr     checkferror
        bne     ret_eof

        ; Push _write parameters
        ldy     #_FILE::f_fd
        lda     (ptr1),y
        ldx     #$00
        jsr     swapstk         ; Push fd, get s

        jsr     pushax          ; Push s

        jsr     _strlen         ; Get length

        ; Write
        jmp     _write

ret_eof:
        jmp     returnFFFF
