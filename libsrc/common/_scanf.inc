;
; Ullrich von Bassewitz, 2004-11-27
;

;----------------------------------------------------------------------------
; Structure passed to _scanf

.struct SCANFDATA
        GET     .addr
        UNGET   .addr
        DATA    .addr
.endstruct

;----------------------------------------------------------------------------
; Global data

.global __scanf




