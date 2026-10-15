;
;GEOS Input Driver Jump Tab
;reassembled by Maciej 'YTM/Alliance' Witkowiak
;4-2-99
;
; 2026: fixed missing InitMouse; corrected the "C128" section

MOUSE_BASE              =       $fe80

InitMouse               =       $fe80
SlowMouse               =       $fe83
UpdateMouse             =       $fe86

MOUSE_BASE_128          =       $fd00

InitMouse_128           =       $fd00
SlowMouse_128           =       $fd03
UpdateMouse_128         =       $fd06
SetMouse_128            =       $fd09
