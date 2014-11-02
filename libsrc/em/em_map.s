;
; Ullrich von Bassewitz, 2002-11-29
;
; void* __fastcall__ em_map (unsigned page);
; /* Unmap the current page from memory and map a new one. The function returns
; ** a pointer to the location of the page in memory.
; */


        .include        "em-kernel.inc"

        _em_map         = emd_map               ; Use driver entry

