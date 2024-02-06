;
; Ullrich von Bassewitz, 2003-12-30
;
; void __fastcall__ mouse_pos (struct mouse_pos* pos);
; /* Return the current mouse position */
;

        .import         ptr1: zp

        .include        "mouse-kernel.inc"

.proc   _mouse_pos

        sta     ptr1
        stx     ptr1+1                  ; Store pos into ptr1
        jmp     mouse_pos               ; Call the driver

.endproc





