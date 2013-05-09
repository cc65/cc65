;
; Karri Kaksonen, 2011
;
; This header contains data for emulators like Handy and Mednafen
;
        .import         __BLOCKSIZE__
        .export         __EXEHDR__: absolute = 1


; ------------------------------------------------------------------------
; EXE header
        .segment "EXEHDR"
        .byte   'L','Y','N','X'                         ; magic
        .word   __BLOCKSIZE__                           ; bank 0 page size
        .word   __BLOCKSIZE__                           ; bank 1 page size
        .word   1                                       ; version number
        .asciiz "Cart name                      "       ; 32 bytes cart name
        .asciiz "Manufacturer   "                       ; 16 bytes manufacturer
        .byte   0                                       ; rotation 1=left
                                                        ; rotation 2=right
        .byte   0,0,0,0,0                               ; spare

