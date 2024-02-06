;
; Ullrich von Bassewitz, 2002-11-29
;
; unsigned em_pagecount (void);
; /* Return the total number of 256 byte pages available in extended memory. */


        .include        "em-kernel.inc"

        _em_pagecount   = emd_pagecount                 ; Use driver entry

