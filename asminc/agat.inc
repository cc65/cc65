
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
CURSOR  :=      $2D     ; Cursor character
TATTR   :=      $32     ; Text attributes
PROMPT  :=      $33     ; Used by GETLN
VCOUT   :=      $36     ; COUT Subroutine Vector
VCIN    :=      $38     ; CIN Subroutine Vector
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

; Game controller
BUTN0   :=      $C061   ; Open-Apple Key
BUTN1   :=      $C062   ; Closed-Apple Key
