;
; Colin Leroy-Mira, 2024
;
; int __fastcall__ fputc (int c, FILE* f);
;

        .export         _fputc
        .importzp       ptr1
        .import         _write, checkferror
        .import         pushax, pusha0, popax, incsp2
        .import         pushptr1, popptr1, returnFFFF

        .include        "stdio.inc"
        .include        "_file.inc"

_fputc:
        sta     ptr1
        stx     ptr1+1

        jsr     popax           ; Get char, as we'll have
        sta     c               ; to return it anyway
        stx     c+1

        jsr     checkferror
        bne     ret_eof

        jsr     pushptr1        ; Backup fp pointer

        ; Push _write parameters
        ldy     #_FILE::f_fd
        lda     (ptr1),y
        jsr     pusha0

        lda     #<c
        ldx     #>c
        jsr     pushax

        lda     #$01
        ldx     #$00

        ; Write
        jsr     _write

        ; Check for errors
        cmp     #$01
        bne     set_ferror

        ; Return char
        lda     c
        ldx     #$00
        jmp     incsp2          ; Drop fp pointer copy

ret_eof:
        jmp     returnFFFF

set_ferror:
        jsr     popptr1
        lda     #_FERROR
        ldy     #_FILE::f_flags
        ora     (ptr1),y
        sta     (ptr1),y
        jmp     returnFFFF

        .bss

c:      .res 2
