;
; Plus/4 generic definitions.
;


; ---------------------------------------------------------------------------
; Zero page, Commodore stuff

ST   		= $90	     	; IEC status byte

FNAM_LEN       	= $AB	     	; Length of filename
SECADR		= $AD	     	; Secondary address
DEVNUM		= $AE	     	; Device number
KEY_COUNT      	= $EF 	     	; Number of keys in input buffer
CURS_X		= $CA  		; Cursor column
CURS_Y		= $CD  		; Cursor row
SCREEN_PTR	= $C8  		; Pointer to current char in text screen
CRAM_PTR	= $EA  		; Pointer to current char in color RAM

CHARCOLOR       = $53B
FKEY_COUNT	= $55D		; Characters for function key
FKEY_SPACE	= $55F		; Function key definitions
FKEY_ORIG	= $F3D2		; Original definitions

; ---------------------------------------------------------------------------
; Kernal routines

; Direct entries
CLRSCR		= $D88B
KBDREAD		= $D8C1

; ---------------------------------------------------------------------------
; Vector and other locations

IRQVec		= $0314
BRKVec		= $0316
NMIVec		= $0318

; ---------------------------------------------------------------------------
; I/O

TED_T1LO      	= $FF00
TED_T1HI	= $FF01
TED_T2LO	= $FF02
TED_T2HI	= $FF03
TED_T3LO	= $FF04
TED_T4HI	= $FF05
TED_KBD		= $FF08
TED_CURSHI	= $FF0C
TED_CURSLO	= $FF0D
TED_V1FRQLO	= $FF0E
TED_V2FRQLO	= $FF0F
TED_V2FRQHI	= $FF10
TED_BGCOLOR	= $FF15
TED_COLOR1	= $FF16
TED_COLOR2	= $FF17
TED_COLOR3	= $FF18
TED_BORDERCOLOR	= $FF19
TED_VLINEHI	= $FF1C
TED_VLINELO	= $FF1D
TED_HPOS	= $FF1E
TED_ROMSEL	= $FF3E
TED_RAMSEL	= $FF3F



