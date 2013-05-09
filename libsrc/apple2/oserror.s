;
; Ullrich von Bassewitz, 17.05.2000
;
; int __fastcall__ _osmaperrno (unsigned char oserror);
;

        .export         __osmaperrno

        .include        "errno.inc"

__osmaperrno:
        ldx     #ErrTabSize
:       cmp     ErrTab-2,x      ; Search for the error code
        beq     :+              ; Jump if found
        dex
        dex
        bne     :-              ; Next entry

        ; Code not found, return EUNKNOWN
        lda     #<EUNKNOWN
        ldx     #>EUNKNOWN
        rts

        ; Found the code
:       lda     ErrTab-1,x
        ldx     #$00            ; High byte always zero
        rts

        .rodata

ErrTab: .byte   $01, ENOSYS     ; Bad system call number
        .byte   $04, EINVAL     ; Bad system call parameter count
        .byte   $25, ENOMEM     ; Interrupt table full
        .byte   $27, EIO        ; I/O error
        .byte   $28, ENODEV     ; No device connected
        .byte   $2B, EACCES     ; Disk write protected
;       .byte   $2E, EUNKNOWN   ; Disk switched
        .byte   $2F, ENODEV     ; Device off-line
        .byte   $40, EINVAL     ; Invalid pathname
        .byte   $42, EMFILE     ; Maximum number of files open
        .byte   $43, EINVAL     ; Invalid reference number
        .byte   $44, ENOENT     ; Directory not found
        .byte   $45, ENOENT     ; Volume not found
        .byte   $46, ENOENT     ; File not found
        .byte   $47, EEXIST     ; Duplicate filename
        .byte   $48, ENOSPC     ; Volume full
        .byte   $49, ENOSPC     ; Volume directory full
;       .byte   $4A, EUNKNOWN   ; Incompatible file format
        .byte   $4B, EINVAL     ; Unsupported storage_type
;       .byte   $4C, EUNKNOWN   ; End of file encountered
        .byte   $4D, ESPIPE     ; Position out of range
        .byte   $4E, EACCES     ; File access error
        .byte   $50, EINVAL     ; File is open
;       .byte   $51, EUNKNOWN   ; Directory structure damaged
        .byte   $52, ENODEV     ; Not a ProDOS volume
        .byte   $53, ERANGE     ; Invalid system call parameter
        .byte   $55, EMFILE     ; Volume Control Block table full
        .byte   $56, EINVAL     ; Bad buffer address
;       .byte   $57, EUNKNOWN   ; Duplicate volume
;       .byte   $5A, EUNKNOWN   ; File structure damaged

ErrTabSize = (* - ErrTab)
