;
; Minimal GEOS input driver skeleton.
;
; 2026, Asbjørn Djupdal (asbjoern@djupdal.org)
;

            .export InitMouse, SlowMouse, UpdateMouse

; ------------------------------------------------------------------------
; The jump table itself must be exactly these 3 3-byte jmp instructions

.segment "STARTUP"

InitMouse:      jmp DoInitMouse
SlowMouse:      jmp DoSlowMouse
UpdateMouse:    jmp DoUpdateMouse

; ------------------------------------------------------------------------

.segment "CODE"

DoInitMouse:
        rts

DoSlowMouse:
        rts

DoUpdateMouse:
        rts

