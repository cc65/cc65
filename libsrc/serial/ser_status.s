;
; Ullrich von Bassewitz, 2003-04-18
;
; unsigned char __fastcall__ ser_status (unsigned char* status);
; /* Return the serial port status. */


        .importzp       ptr1

        .include        "ser-kernel.inc"


.proc   _ser_status

        sta     ptr1
        stx     ptr1+1                  ; Save pointer to status
        jmp     ser_status              ; Call the driver

.endproc


