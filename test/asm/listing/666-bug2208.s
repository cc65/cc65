
; #2208 - Current ca65 generates different code for msbasic

.setcpu "6502"

ZP_START2 = $04

INPUTBUFFERX = INPUTBUFFER & $FF00

;------------------------------------------------------------------------------

;.feature org_per_seg
.zeropage

; if not present, both 2.18 and head produce a absolute,x instruction
; if present, 2.18 produces a zp,x instruction - but head uses abs,x!
.org ZP_START2

INPUTBUFFER:

;------------------------------------------------------------------------------

.segment "CODE"

        .byte $f0, $f1, $f2, $f3    ; just some data to search for
        ; BUG: this will wrongly assembly into a 3-byte instruction
        ;      under certain conditions
        lda     INPUTBUFFERX,x
