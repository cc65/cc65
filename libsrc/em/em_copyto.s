;
; Ullrich von Bassewitz, 2002-11-29
;
; void __fastcall__ em_copyto (const struct em_copy* copy_data);
; /* Copy from linear into extended memory */


        .include        "em-kernel.inc"

        _em_copyto      = emd_copyto            ; Use driver entry

