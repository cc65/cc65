;
; Minimal GEOS printer driver example
;
; 2026, Asbjørn Djupdal (asbjoern@djupdal.org)
;

            .export InitForPrint, StartPrint, PrintBuffer, StopPrint
            .export GetDimensions, PrintASCII, StartASCII, SetNLQ

; ------------------------------------------------------------------------
; The jump table itself must be exactly these 8 3-byte jmp instructions

.segment "STARTUP"

InitForPrint:   jmp DoInitForPrint
StartPrint:     jmp DoStartPrint
PrintBuffer:    jmp DoPrintBuffer
StopPrint:      jmp DoStopPrint
GetDimensions:  jmp DoGetDimensions
PrintASCII:     jmp DoPrintASCII
StartASCII:     jmp DoStartASCII
SetNLQ:         jmp DoSetNLQ

; ------------------------------------------------------------------------
; The actual routines the jump table above calls into.

.segment "CODE"

DoInitForPrint:
        rts

DoStartPrint:
        ldx #0
        clc
        rts

DoPrintBuffer:
        rts

DoStopPrint:
        ldx #0
        clc
        rts

DoGetDimensions:
        lda #0
        ldx #80
        ldy #105
        rts

DoPrintASCII:
        rts

DoStartASCII:
        ldx #0
        clc
        rts

DoSetNLQ:
        rts
