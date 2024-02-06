;
; Ullrich von Bassewitz, 22.06.2002
;
; void __fastcall__ tgi_setdrawpage (unsigned char page);
; /* Set the drawable page */


        .include        "tgi-kernel.inc"

.proc   _tgi_setdrawpage

        cmp     _tgi_pagecount  ; Compare to available pages
        bcs     @L1
        jmp     tgi_setdrawpage ; Call the driver
@L1:    jmp     tgi_inv_arg     ; Invalid argument

.endproc

