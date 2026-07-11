;
; 2023, Rumbledethumps
;
; initenv - stub for getenv()/putenv() support.
; The rp6502 target has no host environment variables,
; so this constructor is intentionally empty.
;

.export initenv

.segment "ONCE"

.proc initenv

    rts

.endproc
