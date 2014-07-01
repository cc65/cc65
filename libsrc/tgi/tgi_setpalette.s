;
; Ullrich von Bassewitz, 23.06.2002
;
; void __fastcall__ tgi_setpalette (const unsigned char* palette);
; /* Set the palette (not available with all drivers/hardware). palette is
; ** a pointer to as many entries as there are colors.
; */
;

        .include        "tgi-kernel.inc"

        .importzp       ptr1

.proc   _tgi_setpalette

        sta     ptr1
        stx     ptr1+1
        jmp     tgi_setpalette          ; Call the driver

.endproc

