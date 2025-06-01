;
; 2020-01-08, Greg King
;
; void __fastcall__ cbm_k_settim (unsigned long timer);
;

        .export         _cbm_k_settim
        .importzp       sreg

        .include        "cbm.inc"


.proc   _cbm_k_settim
        ldy     sreg
        jmp     SETTIM
.endproc
