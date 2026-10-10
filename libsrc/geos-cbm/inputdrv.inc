;
;GEOS Input Driver Jump Tab
;reassembled by Maciej 'YTM/Alliance' Witkowiak
;4-2-99
;
; 2026: fixed missing InitMouse and a wrong "C128" section (C64 and C128
; share the same addresses below; a genuinely separate address range
; exists only for INPUT_128, a distinct filetype for C128-specific
; drivers, not covered here) -- cross-checked against symbols.txt.

MOUSE_BASE              =       $fe80

InitMouse               =       $fe80
SlowMouse               =       $fe83
UpdateMouse             =       $fe86
SetMouse                =       $fe89   ; C128 only
