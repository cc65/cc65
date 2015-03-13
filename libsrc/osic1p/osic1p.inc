; Addresses
C1P_SCR_BASE    := $D000        ; Base of C1P video RAM
INPUTC          := $FD00        ; Input character from keyboard
RESET           := $FF00        ; Reset address, show boot prompt
KBD             := $DF00        ; Polled keyboard register

; Other definitions
C1P_VRAM_SIZE   = $0400         ; Size of C1P video RAM (1 kB)
