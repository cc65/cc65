;
; Ullrich von Bassewitz, 2003-12-30
;
; void mouse_hide (void);
; /* Hide the mouse. The function manages a counter and may be called more than
; ** once. For each call to mouse_hide there must be a call to mouse_show to make
; ** the mouse visible again.
; */
;

        .include        "mouse-kernel.inc"

.proc   _mouse_hide

        lda     _mouse_hidden           ; Get the flag
        bne     @L1                     ; Jump if already invisible
        jsr     mouse_hide              ; Call the driver
@L1:    inc     _mouse_hidden           ; Set the flag to invisible
        rts

.endproc
