;
; Ullrich von Bassewitz, 22.06.2002
;
; void __fastcall__ tgi_setviewpage (unsigned char page);
; /* Set the visible page. */



        .include        "tgi-kernel.inc"

.proc   _tgi_setviewpage

        cmp     _tgi_pagecount  ; Compare to available pages
        bcs     @L1
        jmp     tgi_setviewpage ; Call the driver
@L1:    jmp     tgi_inv_arg     ; Invalid argument

.endproc

