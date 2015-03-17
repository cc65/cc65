;
; Parameters for video output of Superboard /// in 32 columns by 28 lines mode
;

        .export         ScrBase, ScrRamSize
        .export         ScrLo, ScrHi, ScrWidth, ScrHeight
        .export         ScrollLength, ScrollDist, ScrFirstChar

        .include        "../osic1p.inc"
        .include        "../extzp.inc"

; Parameters for Superboard ///
S3_32x28_SCR_BASE       := C1P_SCR_BASE
S3_32x28_SCR_FIRST_CHAR := S3_32x28_SCR_BASE + $80
S3_32x28_SCR_WIDTH      = $20
S3_32x28_SCR_HEIGHT     = $1C
S3_32x28_SCROLL_DIST    = $20
S3_32x28_SCROLL_LEN     = (S3_32x28_SCR_HEIGHT - 1) * S3_32x28_SCROLL_DIST
S3_32x28_VRAM_SIZE      = C1P_VRAM_SIZE

.rodata

ScrBase:                        ; Start address of video RAM, same as for C1P
        .word   S3_32x28_SCR_BASE
ScrRamSize:                     ; Size of video RAM
        .word   S3_32x28_VRAM_SIZE
ScrFirstChar:                   ; Address of first visible character
        .word   S3_32x28_SCR_FIRST_CHAR
ScrWidth:                       ; Characters per line
        .byte   S3_32x28_SCR_WIDTH
ScrHeight:                      ; Lines on screen
        .byte   S3_32x28_SCR_HEIGHT
ScrollLength:                   ; Number of characters to move when scrolling
        .word   S3_32x28_SCROLL_LEN
ScrollDist:                     ; Memory distance to scroll by one line
        .byte   S3_32x28_SCROLL_DIST

; Screen address tables - offset to real screen
ScrLo:  .byte   $80, $A0, $C0, $E0, $00, $20, $40, $60
        .byte   $80, $A0, $C0, $E0, $00, $20, $40, $60
        .byte   $80, $A0, $C0, $E0, $00, $20, $40, $60
        .byte   $80, $A0, $C0, $E0

ScrHi:  .byte   $D0, $D0, $D0, $D0, $D1, $D1, $D1, $D1
        .byte   $D1, $D1, $D1, $D1, $D2, $D2, $D2, $D2
        .byte   $D2, $D2, $D2, $D2, $D3, $D3, $D3, $D3
        .byte   $D3, $D3, $D3, $D3
