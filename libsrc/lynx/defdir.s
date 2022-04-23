;
; Karri Kaksonen, 2011
;
; A default directory with just the main executable.
;
        .include "lynx.inc"
        .import         __STARTOFDIRECTORY__
        .import         __MAIN_START__
        .import         __STARTUP_LOAD__, __BSS_LOAD__
        .import         __BANK0BLOCKSIZE__
        .export         __DEFDIR__: absolute = 1


; ------------------------------------------------------------------------
; Lynx directory
        .segment "DIRECTORY"

__DIRECTORY_START__:
off0 = __STARTOFDIRECTORY__ + (__DIRECTORY_END__ - __DIRECTORY_START__)
blocka = off0 / __BANK0BLOCKSIZE__
; Entry 0 - first executable
block0 = off0 / __BANK0BLOCKSIZE__
len0 = __BSS_LOAD__ - __STARTUP_LOAD__
        .byte   <block0
        .word   off0 & (__BANK0BLOCKSIZE__ - 1)
        .byte   $88
        .word   __MAIN_START__
        .word   len0
__DIRECTORY_END__:
