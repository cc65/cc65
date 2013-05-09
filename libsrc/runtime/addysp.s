;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Increment the stackpointer by value in y
;

        .export         addysp1, addysp
        .importzp       sp

addysp1:
        iny
addysp: pha                     ; Save A
        clc
        tya                     ; Get the value
        adc     sp              ; Add low byte
        sta     sp              ; Put it back
        bcc     @L1             ; If no carry, we're done
        inc     sp+1            ; Inc high byte
@L1:    pla                     ; Restore A
        rts

