;
; Ullrich von Bassewitz, 17.05.2000
;
; int __fastcall__ _osmaperrno (unsigned char oserror);
;

        .export 	__osmaperrno

        .include	"errno.inc"

__osmaperrno:
        ldx	#ErrTabSize
:       cmp	ErrTab-2,x	; Search for the error code
        beq	:+		; Jump if found
        dex
        dex
        bne	:-		; Next entry

        ; Code not found, return EUNKNOWN
        lda	#<EUNKNOWN
        ldx	#>EUNKNOWN
        rts

        ; Found the code
:       lda	ErrTab-1,x
        ldx	#$00		; High byte always zero
        rts

        .rodata

ErrTab: .byte   $01, ENOSYS	; Bad system call number
        .byte	$04, EINVAL	; Bad system call parameter count
        .byte	$25, ENOMEM	; Interrupt vector table full
        .byte	$27, EIO	; I/O error
        .byte	$28, ENODEV	; No device detected/connected
        .byte	$2B, EACCES	; Disk write protected
;       .byte	$2E, EUNKNOWN	; Disk switched
        .byte	$2F, ENODEV	; Device off-line
        .byte	$40, EINVAL	; Invalid pathname syntax
        .byte	$42, EMFILE	; File Control Block table full
        .byte	$43, EINVAL	; Invalid reference number
        .byte	$44, ENOENT	; Path not found
        .byte	$45, ENOENT	; Volume directory not found
        .byte	$46, ENOENT	; File not found
        .byte	$47, EEXIST	; Duplicate filename
        .byte	$48, ENOSPC	; Overrun error
        .byte	$49, ENOSPC	; Volume directory full
;       .byte	$4A, EUNKNOWN	; Incompatible file format
        .byte	$4B, EINVAL	; Unsupported storage_type
;       .byte	$4C, EUNKNOWN	; End of file has been encountered
        .byte	$4D, ESPIPE	; Position out of range
        .byte	$4E, EACCES	; Access error
        .byte	$50, EINVAL	; File is open
;       .byte	$51, EUNKNOWN	; Directory count error
        .byte	$52, ENODEV	; Not a ProDOS disk
        .byte	$53, ERANGE	; Invalid parameter
        .byte	$55, EMFILE	; Volume Control Block table full
        .byte	$56, EINVAL	; Bad buffer address
;       .byte	$57, EUNKNOWN	; Duplicate volume
;       .byte	$5A, EUNKNOWN	; Bit map disk address is impossible

ErrTabSize = (* - ErrTab)
