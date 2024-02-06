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
        .byte           ((.VERSION >> 8) & $0F) + '0'
        .byte           '.'
        .byte           ((.VERSION >> 4) & $0F) + '0'
        .byte           $00

        ; version
        .byte           (.VERSION & $0F) + '0'
        .byte           $00

        ; machine
        .asciiz         "Commodore 510"



