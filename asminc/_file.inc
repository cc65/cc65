;
; _file.inc
;
; (C) Copyright 2002 Ullrich von Bassewitz (uz@cc65.org)
;

; Assembler include file that makes the constants and structures in _file.h
; available for asm code.

; Struct _FILE offsets and size
_FILE_f_fd      = $00
_FILE_f_flags   = $01
_FILE_size      = $02

; Flags field 
_FCLOSED        = $00
_FOPEN          = $01
_FEOF           = $02
_FERROR         = $04
_FPUSHBACK      = $08
                     
; Maximum number of open files (size of table)
FOPEN_MAX       = 8

; File table
.global         __filetab

