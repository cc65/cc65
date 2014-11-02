;
; Callback routine called from the IRQ handler after the ROM IRQ handler
; had been run.
;
; Christian Groessler, 24.04.2014
;
; Check if there was button/joystick activity before and/or after the ROM handler.
; If there was activity, discard the key presses since they are most
; probably "phantom" key presses.

callback:
        ldx     old_key_count
        cpx     KEY_COUNT
        beq     @nokey

        lda     OLD_BUTTONS             ; keypress before?
        bne     @discard_key            ; yes, discard key

        lda     #$7F
        sta     CIA1_PRA
        lda     CIA1_PRB                ; Read joystick #0
        and     #$1F
        eor     #$1F                    ; keypress after
        beq     @nokey                  ; no, probably a real key press

@discard_key:
        stx     KEY_COUNT               ; set old keyboard buffer fill level

@nokey: rts
