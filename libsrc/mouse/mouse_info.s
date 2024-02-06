;
; Ullrich von Bassewitz, 2003-12-30
;
; void __fastcall__ mouse_info (struct mouse_info* info);
; /* Return the state of the mouse buttons and the position of the mouse */
;

        .import         ptr1: zp

        .include        "mouse-kernel.inc"

.proc   _mouse_info

        sta     ptr1
        stx     ptr1+1                  ; Store info into ptr1
        jmp     mouse_info              ; Call the driver

.endproc





