;
; Ullrich von Bassewitz, 06.08.1998
;
; CC65 runtime: Compare < for unsigned ints
;

        .export         tosult00, tosulta0, tosultax
        .import         tosicmp, boolult, return0


tosult00        = return0       ; This is always false

tosulta0:
        ldx     #$00
tosultax:
        jsr     tosicmp         ; Set flags
        jmp     boolult         ; Convert to boolean

