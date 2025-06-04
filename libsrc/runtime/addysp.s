;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Increment the stackpointer by value in y
;

        .export         addysp1, addysp
        .importzp       spc

addysp1:
        iny
addysp: pha                     ; Save A
        clc
        tya                     ; Get the value
        adc     spc              ; Add low byte
        sta     spc              ; Put it back
        bcc     @L1             ; If no carry, we're done
        inc     spc+1            ; Inc high byte
@L1:    pla                     ; Restore A
        rts

