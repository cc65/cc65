;
; 2002-07-18, Ullrich von Bassewitz
; 2022, ChaN
; 2023, Rumbledethumps
;
; Defines the platform-specific error list.
;
; The table is built as a list of entries:
;
;       .byte   entrylen
;       .byte   errorcode
;       .asciiz errormsg
;
; and, terminated by an entry with length zero that is returned if the
; error code could not be found.
;

.export __sys_oserrlist

.include "rp6502.inc"
.include "errno.inc"

;----------------------------------------------------------------------------
; Macros used to generate the list (may get moved to an include file?)

; Regular entry
.macro sys_oserr_entry code, msg
    .local Start, End
Start:
    .byte End - Start
    .byte code
    .asciiz msg
End:
.endmacro

; Sentinel entry
.macro sys_oserr_sentinel msg
    .byte 0  ; Length is always zero
    .byte 0  ; Code is unused
    .asciiz msg
.endmacro

;----------------------------------------------------------------------------
; The error message table

.rodata
__sys_oserrlist:

    sys_oserr_entry ENOENT                 , "No such file or directory"
    sys_oserr_entry ENOMEM                 , "Out of memory"
    sys_oserr_entry EACCES                 , "Permission denied"
    sys_oserr_entry ENODEV                 , "No such device"
    sys_oserr_entry EMFILE                 , "Too many open files"
    sys_oserr_entry EBUSY                  , "Device or resource busy"
    sys_oserr_entry EINVAL                 , "Invalid argument"
    sys_oserr_entry ENOSPC                 , "No space left on device"
    sys_oserr_entry EEXIST                 , "File exists"
    sys_oserr_entry EAGAIN                 , "Try again"
    sys_oserr_entry EIO                    , "I/O error"
    sys_oserr_entry EINTR                  , "Interrupted system call"
    sys_oserr_entry ENOSYS                 , "Function not implemented"
    sys_oserr_entry ESPIPE                 , "Illegal seek"
    sys_oserr_entry ERANGE                 , "Range error"
    sys_oserr_entry EBADF                  , "Bad file number"
    sys_oserr_entry ENOEXEC                , "Exec format error"
    sys_oserr_entry EUNKNOWN               , "Unknown OS specific error"
    sys_oserr_entry FR_DISK_ERR            , "A hard error occurred in the low level disk I/O layer"
    sys_oserr_entry FR_INT_ERR             , "Assertion failed"
    sys_oserr_entry FR_NOT_READY           , "The physical drive cannot work"
    sys_oserr_entry FR_NO_FILE             , "Could not find the file"
    sys_oserr_entry FR_NO_PATH             , "Could not find the path"
    sys_oserr_entry FR_INVALID_NAME        , "The path name format is invalid"
    sys_oserr_entry FR_DENIED              , "Access denied due to prohibited access or directory full"
    sys_oserr_entry FR_EXIST               , "Access denied due to prohibited access"
    sys_oserr_entry FR_INVALID_OBJECT      , "The file/directory object is invalid"
    sys_oserr_entry FR_WRITE_PROTECTED     , "The physical drive is write protected"
    sys_oserr_entry FR_INVALID_DRIVE       , "The logical drive number is invalid"
    sys_oserr_entry FR_NOT_ENABLED         , "The volume has no work area"
    sys_oserr_entry FR_NO_FILESYSTEM       , "There is no valid FAT volume"
    sys_oserr_entry FR_MKFS_ABORTED        , "The f_mkfs() aborted due to any problem"
    sys_oserr_entry FR_TIMEOUT             , "Could not get a grant to access the volume within defined period"
    sys_oserr_entry FR_LOCKED              , "The operation is rejected according to the file sharing policy"
    sys_oserr_entry FR_NOT_ENOUGH_CORE     , "LFN working buffer could not be allocated"
    sys_oserr_entry FR_TOO_MANY_OPEN_FILES , "Number of open files > FF_FS_LOCK"
    sys_oserr_entry FR_INVALID_PARAMETER   , "Given parameter is invalid"
    sys_oserr_sentinel                       "Unknown error"
