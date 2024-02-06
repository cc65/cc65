;
; Ullrich von Bassewitz, 2002-11-29
;
; void __fastcall__ em_copyfrom (const struct em_copy* copy_data);
; /* Copy from extended into linear memory */


        .include        "em-kernel.inc"

        _em_copyfrom    = emd_copyfrom          ; Use driver entry

