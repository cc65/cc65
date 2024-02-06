;
; Ullrich von Bassewitz, 2002-12-01
;
; void em_commit (void);
; /* Commit changes in the memory window to extended storage. If the contents
; ** of the memory window have been changed, these changes may be lost if
; ** em_map, em_copyfrom or em_copyto are called without calling em_commit
; ** first. Note: Not calling em_commit does not mean that the changes are
; ** discarded, it does just mean that some drivers will discard the changes.
; */

        .include        "em-kernel.inc"

        _em_commit      = emd_commit            ; Use driver entry
