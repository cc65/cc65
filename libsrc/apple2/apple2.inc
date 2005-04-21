
;-----------------------------------------------------------------------------
; Zero page stuff

WNDLFT  :=      $20	; Text window left
WNDWDTH :=      $21	; Text window width
WNDTOP  :=      $22	; Text window top
WNDBTM  :=      $23	; Text window bottom+1
CH      :=      $24	; Cursor horizontal position
CV      :=      $25	; Cursor vertical position
BASL    :=      $28	; Text base address low
BASH    :=      $29	; Text base address high
INVFLG  :=      $32	; Normal/inverse(/flash)
PROMPT  :=      $33	; Used by GETLN
RNDL    :=      $4E	; Random counter low
RNDH    :=      $4F	; Random counter high
HIMEM   :=      $73	; Highest available memory address+1

;-----------------------------------------------------------------------------
; Vectors

DOSWARM :=      $03D0	; DOS warmstart vector
BRKVec  :=      $03F0	; Break vector
SOFTEV  :=      $03F2	; Vector for warm start
PWREDUP :=      $03F4	; This must be = EOR #$A5 of SOFTEV+1

;-----------------------------------------------------------------------------
; Hardware

; Keyboard input
KBD     :=      $C000	; Read keyboard
KBDSTRB :=      $C010	; Clear keyboard strobe

; 80 column card switches
CLR80COL:=      $C000	; Disable 80 column store
SET80COL:=      $C001	; Enable 80 column store
CLRALTCHAR :=   $C00E   ; Normal Apple II char set
SETALTCHAR :=   $C00F   ; Norm/inv LC, no flash
ALTCHARSET :=   $C01E   ; >127 if alt charset switched in
RD80VID :=      $C01F   ; >127 if 80 column video enabled

; Video soft switches
MIXCLR  :=      $C052	; Disable 4 lines of text
MIXSET  :=      $C053	; Enable 4 lines of text
LOWSCR  :=      $C054	; Page 1
HISCR   :=      $C055	; Page 2
LORES   :=	$C056	; Lores graphics
HIRES   :=	$C057	; Hires graphics

; Game controller
BUTN0   :=      $C061	; Open-Apple Key
BUTN1   :=      $C062	; Closed-Apple Key
