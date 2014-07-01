;
; Ullrich von Bassewitz, 2002-11-29
;
; void* __fastcall__ em_use (unsigned page);
; /* Tell the driver that the memory window is associated with a given page.
; ** This call is very similar to em_map. The difference is that the driver
; ** does not necessarily transfer the current contents of the extended
; ** memory into the returned window. If you're going to just write to the
; ** window and the current contents of the window are invalid or no longer
; ** use, this call may perform better than em_map.
; */


        .include        "em-kernel.inc"

        _em_use         = emd_use               ; Use driver entry

