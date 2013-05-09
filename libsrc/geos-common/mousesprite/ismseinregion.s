;
; Maciej 'YTM/Elysium' Witkowiak
;
; 21.12.1999, 2.1.2003

; char IsMseInRegion (struct window *mywindow);

            .import _InitDrawWindow
            .export _IsMseInRegion
            
            .include "jumptab.inc"

_IsMseInRegion:
        jsr _InitDrawWindow
        jsr IsMseInRegion
        ldx #0
        rts
