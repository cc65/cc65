;
; Ullrich von Bassewitz, 2002-11-29
;
; void* __fastcall__ em_mapclean (unsigned page);
; /* Unmap the current page from memory and map a new one. The function returns
;  * a pointer to the location of the page in memory. This function differs from
;  * em_map_page() in that it will discard the contents of the currently mapped
;  * page, assuming that the page has not been modified or that the modifications
;  * are no longer needed, if this leads to better performance. NOTE: This does
;  * NOT mean that the contents of currently mapped page are always discarded!
;  */


        .include        "em-kernel.inc"

       	_em_mapclean    = emd_mapclean         ; Use driver entry

