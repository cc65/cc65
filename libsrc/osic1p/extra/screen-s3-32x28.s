;
; Implementation of screen-layout related functions for Superboard ///
;

        .include        "../osiscreen.inc"

S3_SCR_BASE     := $D000        ; Base of Superboard /// video RAM
S3_VRAM_SIZE    = $0400         ; Size of Superboard /// video RAM (1 kB)
S3_SCR_WIDTH    = $20           ; Screen width
S3_SCR_HEIGHT   = $1C           ; Screen height
S3_SCR_FIRSTCHAR = $80          ; Offset of cursor position (0, 0) from base
                                ; of video RAM
S3_SCROLL_DIST  = $20           ; Memory distance for scrolling by one line

osi_screen_funcs S3_SCR_BASE, S3_VRAM_SIZE, S3_SCR_FIRSTCHAR, \
                        S3_SCR_WIDTH, S3_SCR_HEIGHT, S3_SCROLL_DIST
