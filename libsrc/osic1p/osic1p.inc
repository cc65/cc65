; Addresses
SCRNBASE        := $D000        ; Base of video RAM
INPUTC          := $FD00        ; Input character from keyboard
RESET           := $FF00        ; Reset address, show boot prompt

; Other definitions
VIDEORAMSIZE    = $0400         ; Size of C1P video RAM (1 kB)
SCR_LINELEN     = $18           ; screen width - 1
SCR_HEIGHT      = $18           ; screen height - 1
