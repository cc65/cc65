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

ErrTab: .byte   $01, ENOSYS	; Invalid MLI function code number
        .byte	$04, EINVAL	; Incorrect parameter count
        .byte	$25, ENOMEM	; Interrupt table full
        .byte	$27, EIO	; I/O error
        .byte	$28, ENODEV	; No device connected
        .byte	$2B, EACCES	; Write protected
;       .byte	$2E, EUNKNOWN	; Disk switched
        .byte	$2F, ENODEV	; No disk in drive
        .byte	$40, EINVAL	; Invalid pathname syntax
        .byte	$42, EMFILE	; Too many files open
        .byte	$43, EINVAL	; Bad reference number
        .byte	$44, ENOENT	; Bad pathname
        .byte	$45, ENOENT	; Volume not mounted
        .byte	$46, ENOENT	; File not found
        .byte	$47, EEXIST	; File already exists
        .byte	$48, ENOSPC	; Disk full
        .byte	$49, ENOSPC	; Directory full
;       .byte	$4A, EUNKNOWN	; Incompatible ProDOS version
        .byte	$4B, EINVAL	; Unsupported storage type
;       .byte	$4C, EUNKNOWN	; End of file
        .byte	$4D, ESPIPE	; Position past EOF
        .byte	$4E, EACCES	; Access denied
        .byte	$50, EINVAL	; File already open
;       .byte	$51, EUNKNOWN	; File count bad
        .byte	$52, ENODEV	; Not a ProDOS disk
        .byte	$53, ERANGE	; Parameter out of range
        .byte	$55, EMFILE	; Too many devices mounted
        .byte	$56, EINVAL	; Bad buffer address
;       .byte	$57, EUNKNOWN	; Duplicate volume name
;       .byte	$5A, EUNKNOWN	; Damaged disk free space bit map

ErrTabSize = (* - ErrTab)
