;
; Ullrich von Bassewitz, 17.05.2000
;
; int __fastcall__ _osmaperrno (unsigned char oserror);
; /* Map a system specific error into a system independent code */
;

	.export		__osmaperrno
	.include	"errno.inc"

.code

__osmaperrno:
	ldx	#ErrTabSize
@L1:	cmp	ErrTab-2,x	; Search for the error code
    	beq	@L2		; Jump if found
    	dex
    	dex
    	bne	@L1	        ; Next entry

; Code not found, return EINVAL

	lda	#<EINVAL
	ldx	#>EINVAL
	rts

; Found the code

@L2:	lda	ErrTab-1,x
	ldx	#$00		; High byte always zero
	rts

.rodata

ErrTab:
      	.byte	1, EMFILE	; Too many open files
      	.byte	2, EINVAL	; File is open
      	.byte	3, EINVAL	; File not open
      	.byte	4, ENOENT	; File not found
      	.byte	5, ENODEV	; Device not present
      	.byte	6, EINVAL	; File not input
      	.byte	7, EINVAL	; File not output
      	.byte	8, EINVAL	; Filename missing
      	.byte	9, ENODEV	; Ilegal device
;	.byte  20, 		; Read error
;      	.byte  21, 		; Read error
;	.byte  22, 		; Read error
;      	.byte  23, 		; Read error
;      	.byte  24, 		; Read error
;      	.byte  25, 		; Write error
      	.byte  26, EACCES	; Write protect on
;      	.byte  27, 		; Read error
;      	.byte  28, 		; Write error
;      	.byte  29, 		; Disk ID mismatch
;      	.byte  30, 		; Syntax error
;      	.byte  31, 		; Syntax error
;      	.byte  32, 		; Syntax error
       	.byte  33, EINVAL	; Syntax error (invalid file name)
       	.byte  34, EINVAL	; Syntax error (no file given)
;      	.byte  39, 		; Syntax error
;      	.byte  50, 		; Record not present
;      	.byte  51, 		; Overflow in record
;     	.byte  52, 	 	; File too large
      	.byte  60, EINVAL	; Write file open
      	.byte  61, EINVAL	; File not open
      	.byte  62, ENOENT	; File not found
      	.byte  63, EEXIST	; File exists
      	.byte  64, EINVAL	; File type mismatch
;	.byte  65, 		; No block
;	.byte  66, 		; Illegal track or sector
;      	.byte  67, 		; Illegal system track or sector
      	.byte  70, EBUSY 	; No channel
;      	.byte  71, 		; Directory error
;     	.byte  72, 	 	; Disk full
;      	.byte  73, 		; DOS version mismatch
        .byte  74, ENODEV       ; Drive not ready

ErrTabSize = (* - ErrTab)
