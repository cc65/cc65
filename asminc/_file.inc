;
; _file.inc
;
; (C) Copyright 2002 Ullrich von Bassewitz (uz@cc65.org)
;

; Assembler include file that makes the constants and structures in _file.h
; available for asm code.

; Struct _FILE
.struct _FILE
        f_fd        .byte
        f_flags     .byte
        f_pushback  .byte
.endstruct

; Flags field
_FCLOSED        = $00
_FOPEN          = $01
_FEOF           = $02
_FERROR         = $04
_FPUSHBACK      = $08

; File table
.global         __filetab


