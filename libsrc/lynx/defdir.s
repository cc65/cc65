;
; Karri Kaksonen, 2011
;
; A default directory with just the main executable.
;
        .include "lynx.inc"
        .import         __STARTOFDIRECTORY__
        .import         __RAM_START__
        .import         __CODE_SIZE__,__DATA_SIZE__,__RODATA_SIZE__
        .import         __STARTUP_SIZE__,__INIT_SIZE__,__LOWCODE_SIZE__
        .import         __BLOCKSIZE__
        .export         __DEFDIR__: absolute = 1


; ------------------------------------------------------------------------
; Lynx directory
        .segment "DIRECTORY"

__DIRECTORY_START__:
off0=__STARTOFDIRECTORY__+(__DIRECTORY_END__-__DIRECTORY_START__)
blocka=off0/__BLOCKSIZE__
; Entry 0 - first executable
block0=off0/__BLOCKSIZE__
len0=__STARTUP_SIZE__+__INIT_SIZE__+__CODE_SIZE__+__DATA_SIZE__+__RODATA_SIZE__+__LOWCODE_SIZE__
        .byte   <block0
        .word   off0 & (__BLOCKSIZE__ - 1)
        .byte   $88
        .word   __RAM_START__
        .word   len0
__DIRECTORY_END__:

