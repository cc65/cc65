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
       	sys_oserr_entry         $01, "Invalid MLI function code number"
       	sys_oserr_entry         $04, "Incorrect parameter count"
       	sys_oserr_entry         $25, "Interrupt table full"
       	sys_oserr_entry         $27, "I/O error"
       	sys_oserr_entry         $28, "No device connected"
       	sys_oserr_entry         $2B, "Write protected"
       	sys_oserr_entry         $2E, "Disk switched"
       	sys_oserr_entry         $2F, "No disk in drive"
       	sys_oserr_entry         $40, "Invalid pathname syntax"
       	sys_oserr_entry         $42, "Too many files open"
       	sys_oserr_entry         $43, "Bad reference number"
       	sys_oserr_entry         $44, "Bad pathname"
       	sys_oserr_entry         $45, "Volume not mounted"
       	sys_oserr_entry         $46, "File not found"
       	sys_oserr_entry         $47, "File already exists"
       	sys_oserr_entry         $48, "Disk full"
       	sys_oserr_entry         $49, "Directory full"
       	sys_oserr_entry         $4A, "Incompatible ProDOS version"
       	sys_oserr_entry         $4B, "Unsupported storage type"
       	sys_oserr_entry         $4C, "End of file"
       	sys_oserr_entry         $4D, "Position past EOF"
       	sys_oserr_entry         $4E, "Access denied"
       	sys_oserr_entry         $50, "File already open"
       	sys_oserr_entry         $51, "File count bad"
       	sys_oserr_entry         $52, "Not a ProDOS disk"
       	sys_oserr_entry         $53, "Parameter out of range"
       	sys_oserr_entry         $55, "Too many devices mounted"
       	sys_oserr_entry         $56, "Bad buffer address"
       	sys_oserr_entry         $57, "Duplicate volume name"
       	sys_oserr_entry         $5A, "Damaged disk free space bit map"
        sys_oserr_sentinel      "Unknown error"


