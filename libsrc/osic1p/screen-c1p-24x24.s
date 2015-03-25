;
; Implementation of screen-layout related functions for Challenger 1P
;

        .include        "osiscreen.inc"

C1P_SCR_BASE    := $D000        ; Base of C1P video RAM
C1P_VRAM_SIZE   = $0400         ; Size of C1P video RAM (1 kB)
C1P_SCR_WIDTH   = $18           ; Screen width
C1P_SCR_HEIGHT  = $18           ; Screen height
C1P_SCR_FIRSTCHAR = $85         ; Offset of cursor position (0, 0) from base
                                ; of video RAM
C1P_SCROLL_DIST = $20           ; Memory distance for scrolling by one line

osi_screen_funcs C1P_SCR_BASE, C1P_VRAM_SIZE, C1P_SCR_FIRSTCHAR, \
                        C1P_SCR_WIDTH, C1P_SCR_HEIGHT, C1P_SCROLL_DIST
