;
; Vic20 generic definitions. Stolen mostly from c64.inc - Steve Schmidtke
;


; ---------------------------------------------------------------------------
; Zero page, Commodore stuff

ST   		= $90		; IEC status byte

FNAM_LEN	= $B7		; Length of filename
SECADR		= $B9		; Secondary address
DEVNUM		= $BA		; Device number
KEY_COUNT      	= $C6		; Number of keys in input buffer
CURS_FLAG	= $CC		; 1 = cursor off
CURS_BLINK	= $CD		; Blink counter
CURS_CHAR	= $CE		; Character under the cursor
CURS_COLOR      = $287		; Color under the cursor
CURS_STATE	= $CF		; Cursor blink state
SCREEN_PTR	= $D1		; Pointer to current char in text screen
CURS_X		= $D3		; Cursor column
CURS_Y		= $D6		; Cursor row
CRAM_PTR	= $F3		; Pointer to current char in color RAM

CHARCOLOR       = $286
PALFLAG		= $2A6		; $01 = PAL, $00 = NTSC


; ---------------------------------------------------------------------------
; Kernal routines

; Direct entries
CLRSCR		= $E55F		
KBDREAD		= $E5CF		
NAMED_OPEN	= $F495
NAMED_CLOSE	= $F6DA         
PLOTCHAR	= $EAAA	 	; Char in A, color in X

; ---------------------------------------------------------------------------
; Vector and other locations

IRQVec		= $0314
BRKVec		= $0316
NMIVec		= $0318

; ---------------------------------------------------------------------------
; I/O: 6560 VIC

VIC 		= $9000
VIC_COLOR	= $900F

; ---------------------------------------------------------------------------
; I/O: 6522 VIA1

VIA1		= $9110
VIA1_JOY	= $9111
VIA1_DDRB	= $9112
VIA1_DDRA	= $9113

; ---------------------------------------------------------------------------
; I/O: 6522 VIA2

VIA2 		= $9120
VIA2_JOY	= $9120
VIA2_DDRB	= $9122
VIA2_DDRA	= $9123

