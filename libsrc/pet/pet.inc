;
; C64 generic definitions. Stolen from Elite128
;


; ---------------------------------------------------------------------------
; Zero page, Commodore stuff

TIME            = $8D           ; 60HZ clock
MEMSIZE		= $34		; Size of memory installed
ST     	       	= $96  	       	; IEC status byte
SECADR		= $D3  	       	; Secondary address
DEVNUM		= $D4		; Device number
KEY_COUNT      	= $9E  	       	; Number of keys in input buffer
CURS_FLAG	= $A7  	       	; 1 = cursor off
CURS_BLINK	= $A8  	       	; Blink counter
CURS_CHAR	= $A9  	       	; Character under the cursor
CURS_STATE     	= $AA  	       	; Cursor blink state
SCREEN_PTR     	= $C4  	       	; Pointer to current char in text screen
CURS_X 	       	= $C6  	       	; Cursor column
CURS_Y 	       	= $D8		; Cursor row
SCR_LINELEN	= $D5		; Screen line length

KEY_BUF		= $26F 	       	; Keyboard buffer


; ---------------------------------------------------------------------------
; Vector and other locations

IRQVec		= $0090
BRKVec		= $0092
NMIVec		= $0094

