;
; Ullrich von Bassewitz, 2004-10-06
;
; int __fastcall__ putchar (int c);
;

       	.export		_putchar
        .import         _stdout
        .import         _fputc


.code

_putchar:
        lda     #<_stdout
        ldx     #>_stdout
        jmp     _fputc          ; __fastcall__ function



