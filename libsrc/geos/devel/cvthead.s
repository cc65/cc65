
; NOTE THAT EASIER AND SAFER WAY OF GETTING HEADER IS TO GENERATE IT !!!

; Maciej 'YTM/Alliance' Witkowiak
; 28.02.2000

; This is .cvt header for GEOS files, it is recognized by Convert v2.5 for GEOS
; and Star Commander (when copying GEOS files to/from .d64 images)
; This is only an example, and you should customize file header values (such as
; Author, Class, Date, filename) either here for all GEOS apps or manually later
; in GEOS environment using specialized apps or disk editor

; currently only SEQUENTIAL structure is supported, no overlays

; defineable values are marked with ';**' in comment line, please be careful with
; string lengths


;		.org $0400-508		; $0400 - length of .cvt header

		.segment "HEADER"

		.include "../inc/const.inc"

ProgType	= APPLICATION		;** may be one of:
		; APPLICATION
		; ASSEMBLY
		; DESK_ACC (unusable, unless you will fix end address in header before run)
		; PRINTER  (unusable, unless you change $0400 to $7900 here and in crt0.s)
		; INPUT_DEVICE (like above but change $0400 to $fe80, you have $017a bytes)
		; AUTO_EXEC (you need to fit in $0400-$4fff area)
		; INPUT_128 (like INPUT_DEVICE but change $0400 to $fd80, you have $017a bytes)

	    .byte USR | $80		; DOS filetype
	    .word 0			; T&S, will be fixed by converter

	    .byte "filename"		;** DOS filename (16 chars with $a0 padding)
	    .byte $a0,$a0,$a0,$a0,$a0,$a0,$a0,$a0
	    
	    .word 0			; header T&S

	    .byte SEQUENTIAL		; GEOS structure
	    .byte ProgType		; GEOS filetype
	    .byte 00			;** year 2000=00 or 100?
	    .byte 02			;** month
	    .byte 28			;** day
	    .byte 18			;** hour
	    .byte 58			;** minute

	    .word 0			; size in blocks, will be fixed by converter
	    
	    .byte "PRG formatted GEOS file V1.0"
					; converter stamp
	    .res $c4			; some bytes are left
	    
	    .byte 3, 21, 63 | $80	; icon picture header, 63 bytes follow
	
	    ;** hey, uberhacker! edit icon here!!! ;-))    
	    .byte %11111111, %11111111, %11111111
	    .byte %10000000, %00000000, %00000001
	    .byte %10000000, %00000000, %00000001
	    .byte %10000000, %00000000, %00000001
	    .byte %10000000, %00000000, %00000001
	    .byte %10000000, %00000000, %00000001
	    .byte %10000000, %00000000, %00000001
	    .byte %10000000, %00000000, %00000001
	    .byte %10000000, %00000000, %00000001
	    .byte %10000000, %00000000, %00000001
	    .byte %10000000, %00000000, %00000001
	    .byte %10000000, %00000000, %00000001
	    .byte %10000000, %00000000, %00000001
	    .byte %10000000, %00000000, %00000001
	    .byte %10000000, %00000000, %00000001
	    .byte %10000000, %00000000, %00000001
	    .byte %10000000, %00000000, %00000001
	    .byte %10000000, %00000000, %00000001
	    .byte %10000000, %00000000, %00000001
	    .byte %10000000, %00000000, %00000001
	    .byte %11111111, %11111111, %11111111

	    .byte USR | $80				;again DOS type
	    .byte ProgType				;again GEOS type
	    .byte SEQUENTIAL				;structure
	    .word $0400					;ProgStart
	    .word $0400-1				;ProgEnd (needs proper value for DESK_ACC)
	    .word $0400					;ProgExec
	    
	    .byte "Filename"				;**GEOS class (12 chars)
	    .byte $20,$20,$20,$20			; padding with spaces to 12
	    
	    .byte "V1.0",0				;**version
	    .word 0
	    
	    .byte %01000000				;**40/80 columns capability
; B7 B6
; 0  0	- runs under GEOS128 but only in 40 column mode
; 0  1	- runs under GEOS128 in both 40/80 column modes
; 1  0	- does not run under GEOS128
; 1  1  - runs under GEOS128 but only in 80 column mode

	    
	    .byte "Author"				;**author's name (63 chars)
	    .byte 0					;	 +terminator
	    .res (63-7)					; padding to 63
	    
	    .byte "Compiled with cc65"			;**note (95 chars)
	    .byte 0					;	+terminator
	    .res (95-18)				; padding to 95
	    
	    ; end of header, code follows
