;
; Oliver Schmidt, 2005-09-02
;
; unsigned char __fastcall__ ser_ioctl (unsigned char code, void* data);
; /* Driver specific entry. */


        .import         popa
        .importzp       ptr1

        .include        "ser-kernel.inc"


.proc   _ser_ioctl

        sta     ptr1
        stx     ptr1+1                  ; Save pointer to data
        jsr     popa                    ; Get code
        jmp     ser_ioctl               ; Call the driver

.endproc


