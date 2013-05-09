;
; Ullrich von Bassewitz, 2003-12-30
;
; void mouse_show (void);
; /* Show the mouse. See mouse_hide for more information. */
;

        .include        "mouse-kernel.inc"

.proc   _mouse_show

        lda     _mouse_hidden           ; Mouse visible?
        beq     @L1                     ; Jump if yes
        dec     _mouse_hidden           ; Set the flag
        bne     @L1                     ; Jump if still invisible
        jmp     mouse_show              ; Call the driver
@L1:    rts

.endproc
