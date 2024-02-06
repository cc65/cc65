;
; Ullrich von Bassewitz, 2004-10-14
;
; unsigned __fastcall__ tgi_ioctl (unsigned char code, void* data);
; /* Call the driver specific control function. What this function does for
; ** a specific code depends on the driver. The driver will set an error
; ** for unknown codes or values.
; */
;

        .include        "tgi-kernel.inc"

        .import         popa
        .importzp       ptr1


.proc   _tgi_ioctl

        sta     ptr1
        stx     ptr1+1          ; Save val
        jsr     popa            ; Retrieve code
        jmp     tgi_control     ; Call the driver

.endproc


