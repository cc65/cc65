;
; Ullrich von Bassewitz, 15.11.2002
;
; Variables used for CBM file I/O
;

        .export         __curunit
        .export         __filetype


.data

__curunit:
        .byte   8               ; Default is disk

__filetype:
        .byte   'u'             ; Create user files by default

