;
; Callback routine, called from the IRQ handler after the ROM IRQ handler
; has been run.
;
; 2014-04-24, Christian Groessler
; 2020-07-14, Greg King
;
; Check if there was button/joystick activity before and/or after the ROM handler.
; If there was activity, discard the key presses because they are most
; probably "phantom" key presses.

callback:
        ldx     old_key_count
        cpx     KEY_COUNT
        beq     @nokey

        lda     OLD_BUTTONS             ; keypress before?
        bne     @discard_key            ; yes, discard key

        lda     #$FF
        sta     CIA1_PRA
        lda     CIA1_PRB                ; Read joystick #0
        and     #$1F
        eor     #$1F                    ; keypress after
        beq     @nokey                  ; no, probably a real key press

@discard_key:
        stx     KEY_COUNT               ; set old keyboard buffer fill level

@nokey: rts
