;
; Minimal GEOS C128 input driver skeleton.
;
; 2026, Asbjørn Djupdal (asbjoern@djupdal.org)
;

            .export InitMouse_128, SlowMouse_128, UpdateMouse_128, SetMouse_128

; ------------------------------------------------------------------------
; The jump table itself must be exactly these 4 3-byte jmp instructions

.segment "STARTUP"

InitMouse_128:      jmp DoInitMouse
SlowMouse_128:      jmp DoSlowMouse
UpdateMouse_128:    jmp DoUpdateMouse
SetMouse_128:       jmp DoSetMouse

; ------------------------------------------------------------------------

.segment "CODE"

DoInitMouse:
        rts

DoSlowMouse:
        rts

DoUpdateMouse:
        rts

DoSetMouse:
        rts
