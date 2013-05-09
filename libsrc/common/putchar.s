;
; Ullrich von Bassewitz, 2004-10-06
;
; int __fastcall__ putchar (int c);
;

        .export         _putchar
        .import         pushax
        .import         _stdout
        .import         _fputc


.code

_putchar:
        jsr     pushax          ; Push c
        lda     _stdout
        ldx     _stdout+1
        jmp     _fputc          ; __fastcall__ function



