
;-----------------------------------------------------------------------------
; Zero page stuff

WNDWDTH	:=	$21	; Right edge of text window
CH	:=	$24	; Cursor horizontal position
CV	:=	$25	; Cursor vertical position
BASL	:=	$28	; Text base address
INVFLG	:=	$32	; Normal/inverse(/flash)
RNDL	:=	$4E	; Random counter low
RNDH	:=	$4F	; Random counter high
HIMEM	:=	$73	; Highest available memory address+1

;-----------------------------------------------------------------------------
; Vectors

DOSWARM	:=	$03D0	; DOS warmstart vector
BRKVec	:=	$03F0	; Break vector
MLI	:=	$BF00	; ProDOS Machine Language Interface

;-----------------------------------------------------------------------------
; Hardware

; Keyboard input
KBD	:=	$C000	; Read keyboard
KBDSTRB	:=	$C010	; Clear keyboard strobe

; 80 column card switches
CLRALTCHAR :=	$C00E	; Normal Apple II char set
SETALTCHAR :=	$C00F	; Norm/inv LC, no flash
ALTCHARSET :=	$C01E	; >127 if alt charset switched in
RD80VID :=	$C01F	; >127 if 80 column video enabled

; Video soft switches
MIXCLR	:=	$C052	; Disable 4 lines of text
MIXSET	:=	$C053	; Enable 4 lines of text
LOWSCR	:=	$C054	; Page 1
HISCR	:=	$C055	; Page 2

; Game controller
BUTN0	:=	$C061	; Open-Apple Key
BUTN1	:=	$C062	; Closed-Apple Key
