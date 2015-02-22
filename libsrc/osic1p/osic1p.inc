; Addresses
SCRNBASE        := $D000        ; Base of video RAM
INPUTC          := $FD00        ; Input character from keyboard
RESET           := $FF00        ; Reset address, show boot prompt
KBD             := $DF00        ; Polled keyboard register

; Other definitions
VIDEORAMSIZE    = $0400         ; Size of C1P video RAM (1 kB)
SCR_LINELEN     = $19           ; screen width
SCR_HEIGHT      = $19           ; screen height
