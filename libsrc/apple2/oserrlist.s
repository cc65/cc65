;
; Stefan Haubenthal, 2003-12-24
; Ullrich von Bassewitz, 18.07.2002
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
        sys_oserr_entry         $01, "Bad system call number"
        sys_oserr_entry         $04, "Bad system call parameter count"
        sys_oserr_entry         $25, "Interrupt table full"
        sys_oserr_entry         $27, "I/O error"
        sys_oserr_entry         $28, "No device connected"
        sys_oserr_entry         $2B, "Disk write protected"
        sys_oserr_entry         $2E, "Disk switched"
        sys_oserr_entry         $2F, "Device off-line"
        sys_oserr_entry         $40, "Invalid pathname"
        sys_oserr_entry         $42, "Maximum number of files open"
        sys_oserr_entry         $43, "Invalid reference number"
        sys_oserr_entry         $44, "Directory not found"
        sys_oserr_entry         $45, "Volume not found"
        sys_oserr_entry         $46, "File not found"
        sys_oserr_entry         $47, "Duplicate filename"
        sys_oserr_entry         $48, "Volume full"
        sys_oserr_entry         $49, "Volume directory full"
        sys_oserr_entry         $4A, "Incompatible file format"
        sys_oserr_entry         $4B, "Unsupported storage_type"
        sys_oserr_entry         $4C, "End of file encountered"
        sys_oserr_entry         $4D, "Position out of range"
        sys_oserr_entry         $4E, "File access error"
        sys_oserr_entry         $50, "File is open"
        sys_oserr_entry         $51, "Directory structure damaged"
        sys_oserr_entry         $52, "Not a ProDOS disk"
        sys_oserr_entry         $53, "Invalid system call parameter"
        sys_oserr_entry         $55, "Volume Control Block table full"
        sys_oserr_entry         $56, "Bad buffer address"
        sys_oserr_entry         $57, "Duplicate volume"
        sys_oserr_entry         $5A, "File structure damaged"
        sys_oserr_sentinel      "Unknown error"


