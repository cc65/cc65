;
; Ullrich von Bassewitz, 2003-04-18
;
; unsigned char __fastcall__ ser_params (const struct ser_params* params);
; /* Set the port parameters. This will also enable the port. */


        .importzp       ptr1

        .include        "ser-kernel.inc"



.proc   _ser_params

        sta     ptr1
        stx     ptr1+1                  ; Save pointer to params
        jmp     ser_params              ; Call the driver

.endproc




