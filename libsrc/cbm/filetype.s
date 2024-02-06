;
; Ullrich von Bassewitz, 2012-07-15
;
; Default file type used when creating new files
;

        .export         __filetype


.data

__filetype:
        .byte   's'             ; Create sequential files by default


