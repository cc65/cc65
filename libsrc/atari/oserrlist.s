;
; Ullrich von Bassewitz, 18.07.2002
; Christian Groessler, 24.07.2002
;
; Defines the platform specific error list.
;
; The table is built as a list of entries
;
;       .byte   entrylen
;       .byte   errorcode
;       .asciiz errormsg
;
; and terminated by an entry with length zero that is returned if the
; error code could not be found.
;

        .export         __sys_oserrlist

;----------------------------------------------------------------------------
; Macros used to generate the list (may get moved to an include file?)

; Regular entry
.macro  sys_oserr_entry         code, msg
        .local  Start, End
Start:  .byte   End - Start
        .byte   code
        .asciiz msg
End:
.endmacro

; Sentinel entry
.macro  sys_oserr_sentinel      msg
        .byte   0                       ; Length is always zero
        .byte   0                       ; Code is unused
        .asciiz msg
.endmacro

;----------------------------------------------------------------------------
; The error message table

.rodata

__sys_oserrlist:
        sys_oserr_entry          1, "no error"
        sys_oserr_entry        128, "BREAK key abort"
        sys_oserr_entry        129, "IOCB already open"
        sys_oserr_entry        130, "device not found"
        sys_oserr_entry        131, "IOCB write only"
        sys_oserr_entry        132, "invalid command"
        sys_oserr_entry        133, "IOCB not open"
        sys_oserr_entry        134, "invalid IOCB index"
        sys_oserr_entry        135, "IOCB read only"
        sys_oserr_entry        136, "end-of-file"
        sys_oserr_entry        137, "record truncated"
        sys_oserr_entry        138, "device timeout"
        sys_oserr_entry        139, "device nak"
        sys_oserr_entry        140, "SIO frame error"
        sys_oserr_entry        141, "cursor out of range"
        sys_oserr_entry        142, "SIO data overrun"
        sys_oserr_entry        143, "SIO checksum mismatch"
        sys_oserr_entry        144, "general device failure"
        sys_oserr_entry        145, "bad screen mode"
        sys_oserr_entry        146, "invalid function"
        sys_oserr_entry        147, "insufficient memory for mode"
        sys_oserr_entry        148, "invalid disk format"
        sys_oserr_entry        149, "disk format version mismatch"
        sys_oserr_entry        150, "R: already open/dir not found"
        sys_oserr_entry        151, "concurrent mode not enabled/file exists"
        sys_oserr_entry        152, "concurrent mode invalid buffer address/not binary"
        sys_oserr_entry        153, "concurrent mode enabled"
        sys_oserr_entry        154, "concurrent mode not active/loader symbol not defined"
        sys_oserr_entry        156, "invalid parameter"
        sys_oserr_entry        158, "insufficient memory"
        sys_oserr_entry        160, "drive number error"
        sys_oserr_entry        161, "too many open files"
        sys_oserr_entry        162, "no space left on device"
        sys_oserr_entry        163, "unrecoverable system data I/O error"
        sys_oserr_entry        164, "file number mismatch"
        sys_oserr_entry        165, "invalid file name"
        sys_oserr_entry        166, "point data length error"
        sys_oserr_entry        167, "file read-only"
        sys_oserr_entry        168, "invalid command for disk"
        sys_oserr_entry        169, "directory full"
        sys_oserr_entry        170, "file not found"
        sys_oserr_entry        171, "invalid point command"
        sys_oserr_entry        172, "already exists in parent directory"
        sys_oserr_entry        173, "bad disk"
        sys_oserr_entry        174, "directory not in parent directory"
        sys_oserr_entry        175, "directory not empty"
        sys_oserr_entry        176, "invalid disk format"
        sys_oserr_entry        180, "not a binary file"
        sys_oserr_entry        181, "invalid address range"
        sys_oserr_entry        182, "invalid parameter"
        sys_oserr_sentinel     "unknown error"
