;
; 2000-05-17, Ullrich von Bassewitz
; 2014-05-28, Greg King
;
; int __fastcall__ _osmaperrno (unsigned char oserror);
; /* Map a system-specific error into a system-independent code. */
;

        .include        "errno.inc"

.code

__osmaperrno:
        ldx     #ErrTabSize
@L1:    cmp     ErrTab-2,x      ; Search for the error code
        beq     @L2             ; Jump if found
        dex
        dex
        bne     @L1             ; Next entry

; Code not found, return EUNKNOWN

        lda     #<EUNKNOWN
        ldx     #>EUNKNOWN
        rts

; Found the code

@L2:    lda     ErrTab-1,x
        ldx     #$00            ; High byte always zero
        rts

.rodata

ErrTab:
        .byte   1, EMFILE       ; Too many open files
        .byte   2, EINVAL       ; File is open
        .byte   3, EINVAL       ; File not open
        .byte   4, ENOENT       ; File not found
        .byte   5, ENODEV       ; Device not present
        .byte   6, EINVAL       ; File not input
        .byte   7, EINVAL       ; File not output
        .byte   8, EINVAL       ; Filename missing
        .byte   9, ENODEV       ; Illegal device
        .byte  20, EBUSY        ; No sector header
        .byte  21, EBUSY        ; No sync mark
        .byte  22, EIO          ; No sector data
        .byte  23, EIO          ; Checksum error
        .byte  24, EIO          ; Decode error
        .byte  25, EIO          ; Verify error
        .byte  26, EACCES       ; Write protected
        .byte  27, EIO          ; Checksum error
        .byte  28, EIO          ; Write overrun
        .byte  29, EBUSY        ; Disk ID mismatch
        .byte  30, EINVAL       ; Command not recognized
        .byte  31, ENOSYS       ; Command not implemented
        .byte  32, EINVAL       ; Command too long
        .byte  33, EINVAL       ; Invalid write filename
        .byte  34, EINVAL       ; No file given
        .byte  39, ENOENT       ; System file not found
        .byte  49, EACCES       ; Invalid format
        .byte  50, ESPIPE       ; Record not present
        .byte  51, ENOSPC       ; Overflow in record
        .byte  52, ENOSPC       ; File too large
        .byte  60, EBUSY        ; Write file open
        .byte  61, EINVAL       ; File not open
        .byte  62, ENOENT       ; File not found
        .byte  63, EEXIST       ; File exists
        .byte  64, EINVAL       ; File type mismatch
        .byte  65, ESPIPE       ; No block
        .byte  66, EINVAL       ; Illegal track or sector
        .byte  67, EIO          ; Illegal system track or sector
        .byte  70, EBUSY        ; No channel
        .byte  71, EIO          ; BAM error
        .byte  72, ENOSPC       ; Disk full
        .byte  73, EACCES       ; DOS version mismatch
        .byte  74, ENODEV       ; Drive not ready
        .byte  75, EIO          ; Format error
        .byte  77, EINVAL       ; Illegal partition
        .byte  78, EIO          ; Bad system area

ErrTabSize = (* - ErrTab)
