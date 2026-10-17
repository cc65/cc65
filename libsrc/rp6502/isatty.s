;
; int __fastcall__ isatty (int fd);
;

        .export         _isatty
        .import         return0, return1

; stdin, stdout, stderr, CON: and TTY: are descriptors 0 to 4, all the console.
_isatty:
        cpx     #$00
        bne     @no
        cmp     #$05
        bcs     @no
        jmp     return1
@no:    jmp     return0
