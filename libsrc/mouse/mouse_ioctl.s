;
; Ullrich von Bassewitz, 2003-12-30
;
; unsigned char __fastcall__ mouse_ioctl (unsigned char code, void* data);
; /* Call the driver specific ioctl function. NON PORTABLE! Returns an error
; ** code.
; */
;

        .import         popa
        .import         ptr1: zp

        .include        "mouse-kernel.inc"

.proc   _mouse_ioctl

        sta     ptr1
        stx     ptr1+1                  ; Store data into ptr1
        jsr     popa                    ; Get code from stack
        jmp     mouse_ioctl             ; Call the driver

.endproc





