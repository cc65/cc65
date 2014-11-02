;
; Ullrich von Bassewitz, 2003-04-18
;
; unsigned char __fastcall__ ser_get (char* b);
; /* Get a character from the serial port. If no characters are available, the
; ** function will return SER_ERR_NO_DATA, so this is not a fatal error.
; */


        .importzp       ptr1

        .include        "ser-kernel.inc"


.proc   _ser_get

        sta     ptr1
        stx     ptr1+1                  ; Save pointer to char
        jmp     ser_get                 ; Call the driver

.endproc




