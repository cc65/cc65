
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

LASTCHAR :=     $D2     ; last character Read

;-----------------------------------------------------------------------------
; ROM functions

READCHARUPPER := $F643
READCHARASYNC := $F651

;-----------------------------------------------------------------------------
; Hardware

