;
; Default parameters for video output, tailored to the
; Superboard II/Challenger 1P
;
; Can be overridden for other screen layouts with other object files from
; the "extra" directory
;

        .export         ScrBase, ScrRamSize
        .export         ScrLo, ScrHi, ScrWidth, ScrHeight
        .export         ScrollLength, ScrollDist, ScrFirstChar

        .include        "osic1p.inc"
        .include        "extzp.inc"

; Parameters for OSI Challenger 1P
C1P_SCR_FIRST_CHAR := C1P_SCR_BASE + $85
C1P_SCR_WIDTH   = $18
C1P_SCR_HEIGHT  = $18
C1P_SCROLL_DIST = $20
C1P_SCROLL_LEN  = (C1P_SCR_HEIGHT - 1) * C1P_SCROLL_DIST

.rodata

ScrBase:                        ; Start address of video RAM
        .word   C1P_SCR_BASE
ScrRamSize:                     ; Size of video RAM
        .word   C1P_VRAM_SIZE
ScrFirstChar:                   ; Address of first visible character
        .word   C1P_SCR_FIRST_CHAR
ScrWidth:                       ; Characters per line
        .byte   C1P_SCR_WIDTH
ScrHeight:                      ; Lines on screen
        .byte   C1P_SCR_HEIGHT
ScrollLength:                   ; Number of characters to move when scrolling
        .word   C1P_SCROLL_LEN
ScrollDist:                     ; Memory distance to scroll by one line
        .byte   C1P_SCROLL_DIST

; Screen address tables - offset to real screen
ScrLo:  .byte   $85, $A5, $C5, $E5, $05, $25, $45, $65
        .byte   $85, $A5, $C5, $E5, $05, $25, $45, $65
        .byte   $85, $A5, $C5, $E5, $05, $25, $45, $65
        .byte   $85

ScrHi:  .byte   $D0, $D0, $D0, $D0, $D1, $D1, $D1, $D1
        .byte   $D1, $D1, $D1, $D1, $D2, $D2, $D2, $D2
        .byte   $D2, $D2, $D2, $D2, $D3, $D3, $D3, $D3
        .byte   $D3
