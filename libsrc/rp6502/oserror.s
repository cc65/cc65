;
; 2000-05-17, Ullrich von Bassewitz
; 2022, ChaN
; 2023, Rumbledethumps
;
; int __fastcall__ __osmaperrno (unsigned char oserror);
;
; RP6502 will respond with a union of CC65 and FatFs errnos.
; This will map FatFs errors into the CC65 range for portable code.

EFATFS_START := 32

.include "rp6502.inc"
.include "errno.inc"

.code

___osmaperrno:
    cmp #EFATFS_START
    bmi @L2

    ldx #ErrTabSize
@L1:
    cmp ErrTab-2,x      ; Search for the error code
    beq @L3             ; Jump if found
    dex
    dex
    bne @L1             ; Next entry

; Code not found, return EUNKNOWN
    lda #<EUNKNOWN
    ldx #>EUNKNOWN
@L2:
    rts

; Found the code
@L3:
    lda ErrTab-1,x
    ldx #$00            ; High byte always zero
    rts

.rodata

ErrTab:

    .byte FR_DISK_ERR            , EIO       ; A hard error occurred in the low level disk I/O layer
;   .byte FR_INT_ERR             , EUNKNOWN  ; Assertion failed
    .byte FR_NOT_READY           , EBUSY     ; The physical drive cannot work
    .byte FR_NO_FILE             , ENOENT    ; Could not find the file
    .byte FR_NO_PATH             , ENOENT    ; Could not find the path
    .byte FR_INVALID_NAME        , EINVAL    ; The path name format is invalid
    .byte FR_DENIED              , EACCES    ; Access denied due to prohibited access or directory full
    .byte FR_EXIST               , EEXIST    ; Access denied due to prohibited access
    .byte FR_INVALID_OBJECT      , EINVAL    ; The file/directory object is invalid
    .byte FR_WRITE_PROTECTED     , EACCES    ; The physical drive is write protected
    .byte FR_INVALID_DRIVE       , ENODEV    ; The logical drive number is invalid
;   .byte FR_NOT_ENABLED         , EUNKNOWN  ; The volume has no work area
;   .byte FR_NO_FILESYSTEM       , EUNKNOWN  ; There is no valid FAT volume
;   .byte FR_MKFS_ABORTED        , EUNKNOWN  ; The f_mkfs() aborted due to any problem
;   .byte FR_TIMEOUT             , EUNKNOWN  ; Could not get a grant to access the volume within defined period
    .byte FR_LOCKED              , EBUSY     ; The operation is rejected according to the file sharing policy
    .byte FR_NOT_ENOUGH_CORE     , ENOMEM    ; LFN working buffer could not be allocated
    .byte FR_TOO_MANY_OPEN_FILES , EMFILE    ; Number of open files > FF_FS_LOCK
    .byte FR_INVALID_PARAMETER   , EINVAL    ; Given parameter is invalid

ErrTabSize = (* - ErrTab)
