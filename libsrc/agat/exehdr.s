;
; Oliver Schmidt, 2012-06-10
;
; This module supplies an AppleSingle version 2 file header + entry with
; ID 11 according to https://tools.ietf.org/rfc/rfc1740.txt Appendix A.
;

        .export         __EXEHDR__ : absolute = 1       ; Linker referenced
        .import         __FILETYPE__                    ; Linker generated
        .import         __MAIN_START__, __MAIN_LAST__   ; Linker generated

; ------------------------------------------------------------------------

; Data Fork
ID01_LENGTH = __MAIN_LAST__ - __MAIN_START__
ID01_OFFSET = ID01 - START

; ProDOS File Info
ID11_LENGTH = ID01 - ID11
ID11_OFFSET = ID11 - START

; ------------------------------------------------------------------------

        .segment        "EXEHDR"

START:  .byte           $00, $05, $16, $00                  ; Magic number
        .byte           $00, $02, $00, $00                  ; Version number
        .res            16                                  ; Filler
        .byte           0, 2                                ; Number of entries
        .byte           0, 0, 0, 1                          ; Entry ID 1 - Data Fork
        .byte           0, 0, >ID01_OFFSET, <ID01_OFFSET    ; Offset
        .byte           0, 0, >ID01_LENGTH, <ID01_LENGTH    ; Length
        .byte           0, 0, 0, 11                         ; Entry ID 11 - ProDOS File Info
        .byte           0, 0, >ID11_OFFSET, <ID11_OFFSET    ; Offset
        .byte           0, 0, >ID11_LENGTH, <ID11_LENGTH    ; Length
ID11:   .byte           0, %11000011                        ; Access - Destroy, Rename, Write, Read
        .byte           >__FILETYPE__, <__FILETYPE__        ; File Type
        .byte           0, 0                                ; Auxiliary Type high
        .byte           >__MAIN_START__, <__MAIN_START__    ; Auxiliary Type low
ID01:
