
;-----------------------------------------------------------------------------
; Zero page

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
CURLIN  :=      $75     ; Current line number being executed

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

; 80 column video
CLR80COL:=      $C000   ; Disable 80 column store
SET80COL:=      $C001   ; Enable 80 column store
RD80COL :=      $C018   ; >127 if 80 column store enabled
RD80VID :=      $C01F   ; >127 if 80 column video enabled

; Character set
CLRALTCHAR :=   $C00E   ; Normal Apple II char set
SETALTCHAR :=   $C00F   ; Norm/inv LC, no flash
ALTCHARSET :=   $C01E   ; >127 if alt charset switched in

; Language card
RDLCBNK2:=      $C011   ; >127 if LC bank 2 in use
RDLCRAM :=      $C012   ; >127 if LC is read enabled
ROMIN   :=      $C081   ; Swap in D000-FFFF ROM
LCBANK2 :=      $C083   ; Swap in LC bank 2
LCBANK1 :=      $C08B   ; Swap in LC bank 1

; Vertical blanking
RDVBLBAR    :=  $C019   ; >127 if not vertical blanking
RDVBLMSK    :=  $C041   ; >127 if VBL interrupts enabled
DISVBL      :=  $C05A   ; Disable VBL interrupts
ENVBL       :=  $C05B   ; Enable VBL interrupts

; Video mode
TXTCLR      :=  $C050   ; Display graphics
TXTSET      :=  $C051   ; Display text
MIXCLR      :=  $C052   ; Disable 4 lines of text
MIXSET      :=  $C053   ; Enable 4 lines of text
LOWSCR      :=  $C054   ; Page 1
HISCR       :=  $C055   ; Page 2
LORES       :=  $C056   ; Lores graphics
HIRES       :=  $C057   ; Hires graphics
DHIRESON    :=  $C05E   ; Enable double-width graphics
DHIRESOFF   :=  $C05F   ; Disable double-width graphics

; Game controller
TAPEIN  :=      $C060   ; Read casette input / Switch input 3
BUTN0   :=      $C061   ; Switch input 0 / Open-Apple key
BUTN1   :=      $C062   ; Switch input 1 / Closed-Apple key
BUTN2   :=      $C063   ; Switch input 2 / Shift key
PADDL0  :=      $C064   ; Analog input 0
PADDL1  :=      $C065   ; Analog input 1
PADDL2  :=      $C066   ; Analog input 2
PADDL3  :=      $C067   ; Analog input 3
PTRIG   :=      $C070   ; Analog input reset

; Input/Output Unit
IOUDISON    :=  $C07E   ; Disable IOU
IOUDISOFF   :=  $C07F   ; Enable IOU

; Control Your Apple
CYAREG  :=      $C036   ; Bits 0-3=disk detect 4=shadow all banks 7=fast
