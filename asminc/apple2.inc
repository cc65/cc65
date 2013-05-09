
;-----------------------------------------------------------------------------
; Zero page stuff

WNDLFT  :=      $20     ; Text window left
WNDWDTH :=      $21     ; Text window width
WNDTOP  :=      $22     ; Text window top
WNDBTM  :=      $23     ; Text window bottom+1
CH      :=      $24     ; Cursor horizontal position
CV      :=      $25     ; Cursor vertical position
BASL    :=      $28     ; Text base address low
BASH    :=      $29     ; Text base address high
INVFLG  :=      $32     ; Normal/inverse(/flash)
PROMPT  :=      $33     ; Used by GETLN
RNDL    :=      $4E     ; Random counter low
RNDH    :=      $4F     ; Random counter high
HIMEM   :=      $73     ; Highest available memory address+1

;-----------------------------------------------------------------------------
; Vectors

DOSWARM :=      $03D0   ; DOS warmstart vector
BRKVec  :=      $03F0   ; Break vector
SOFTEV  :=      $03F2   ; Vector for warm start
PWREDUP :=      $03F4   ; This must be = EOR #$A5 of SOFTEV+1

;-----------------------------------------------------------------------------
; Hardware

; Keyboard input
KBD     :=      $C000   ; Read keyboard
KBDSTRB :=      $C010   ; Clear keyboard strobe

; 80 column video switches
CLR80COL:=      $C000   ; Disable 80 column store
SET80COL:=      $C001   ; Enable 80 column store
RD80COL :=      $C018   ; >127 if 80 column store enabled
RD80VID :=      $C01F   ; >127 if 80 column video enabled

; Character set switches
CLRALTCHAR :=   $C00E   ; Normal Apple II char set
SETALTCHAR :=   $C00F   ; Norm/inv LC, no flash
ALTCHARSET :=   $C01E   ; >127 if alt charset switched in

; Language card switches
RDLCBNK2:=      $C011   ; >127 if LC bank 2 in use
RDLCRAM :=      $C012   ; >127 if LC is read enabled
ROMIN   :=      $C081   ; Swap in D000-FFFF ROM
LCBANK2 :=      $C083   ; Swap in LC bank 2
LCBANK1 :=      $C08B   ; Swap in LC bank 1

; Video mode switches
TXTCLR  :=      $C050   ; Display graphics
TXTSET  :=      $C051   ; Display text
MIXCLR  :=      $C052   ; Disable 4 lines of text
MIXSET  :=      $C053   ; Enable 4 lines of text
LOWSCR  :=      $C054   ; Page 1
HISCR   :=      $C055   ; Page 2
LORES   :=      $C056   ; Lores graphics
HIRES   :=      $C057   ; Hires graphics

; Game controller
BUTN0   :=      $C061   ; Open-Apple Key
BUTN1   :=      $C062   ; Closed-Apple Key
