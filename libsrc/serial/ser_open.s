;
; Ullrich von Bassewitz, 2003-08-22
;
; unsigned char __fastcall__ ser_open (const struct ser_params* params);
; /* "Open" the port by setting the port parameters and enable interrupts. */



        .importzp       ptr1

        .include        "ser-kernel.inc"



.proc   _ser_open

        sta     ptr1
        stx     ptr1+1                  ; Save pointer to params
        jmp     ser_open                ; Call the driver

.endproc



