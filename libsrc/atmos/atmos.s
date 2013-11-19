;
; Expose include-file symbol names to C code.
;

        .export         _atmos_ping, _atmos_shoot, _atmos_explode
        .export         _atmos_zap, _atmos_tick, _atmos_tock

        .include        "atmos.inc"

_atmos_ping     := PING
_atmos_shoot    := SHOOT
_atmos_explode  := EXPLODE
_atmos_zap      := ZAP
_atmos_tick     := TICK
_atmos_tock     := TOCK
