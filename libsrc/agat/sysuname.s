;
; Ullrich von Bassewitz, 2003-08-12
;
; unsigned char __fastcall__ _sysuname (struct utsname* buf);
;

        .export         __sysuname, utsdata

        .import         utscopy

        __sysuname = utscopy

;--------------------------------------------------------------------------
; Data. We define a fixed utsname struct here and just copy it.

.rodata

utsdata:
        ; sysname
        .asciiz         "cc65"

        ; nodename
        .asciiz         ""

        ; release
        .byte           .string (>.version)
        .byte           '.'
        .byte           .string (<.version)
        .byte           $00

        ; version
        .byte           '0'     ; unused
        .byte           $00

        ; machine
        .asciiz         "Agat"

