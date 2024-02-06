;
; P/M mouse shape default definition
;
; Christian Groessler, 11.04.2014
;
; Note that the height of the mouse cursor must not exceed 32
; lines, otherwise the display routines won't do The Right
; Thing(tm).
;

        .export mouse_pm_bits
        .export mouse_pm_height    : zeropage
        .export mouse_pm_hotspot_x : zeropage
        .export mouse_pm_hotspot_y : zeropage

        .rodata

mouse_pm_bits:
        .byte   %11110000
        .byte   %11000000
        .byte   %10100000
        .byte   %10010000
        .byte   %10001000
        .byte   %00000100
        .byte   %00000010

mouse_pm_height = <(* - mouse_pm_bits)

; hot spot is upper left corner
mouse_pm_hotspot_x = 0
mouse_pm_hotspot_y = 0
