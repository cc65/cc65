;
; Stefan Haubenthal, 2004-05-25
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
        sys_oserr_entry          1, "File not found"
        sys_oserr_entry          2, "Invalid command end"
        sys_oserr_entry          3, "No drive number"
        sys_oserr_entry          4, "Bad drive number"
        sys_oserr_entry          5, "Invalid filename"
        sys_oserr_entry          6, "fderr=(error  number)"
        sys_oserr_entry          7, "Illegal attribute"
        sys_oserr_entry          8, "Wildcard(s) not allowed"
        sys_oserr_entry          9, "File already exists"
        sys_oserr_entry         10, "Insufficient disc space"
        sys_oserr_entry         11, "File open"
        sys_oserr_entry         12, "Illegal quantity"
        sys_oserr_entry         13, "End address missing"
        sys_oserr_entry         14, "Start address > end address"
        sys_oserr_entry         15, "Missing 'to'"
        sys_oserr_entry         16, "Renamed file not on same disc"
        sys_oserr_entry         17, "Unknown array"
        sys_oserr_entry         18, "Target drive not source drive"
        sys_oserr_entry         19, "Destination not specified"
        sys_oserr_entry         20, "Cannot merge and overwrite"
        sys_oserr_entry         21, "Single target file illegal"
        sys_oserr_entry         22, "Syntax"
        sys_oserr_entry         23, "Filename missing"
        sys_oserr_entry         24, "Source file missing"
        sys_oserr_entry         25, "Type mismatch"
        sys_oserr_entry         26, "Disc write-protected"
        sys_oserr_entry         27, "Incompatible drives"
        sys_oserr_entry         28, "File not open"
        sys_oserr_entry         29, "File end"
        sys_oserr_sentinel      "Unknown error"


