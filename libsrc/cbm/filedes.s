;
; Ullrich von Bassewitz, 16.11.2002
;
; File descriptor management for the POSIX I/O routines
;


        .include        "filedes.inc"

.code

;--------------------------------------------------------------------------
; freefd: Find a free file handle and return it in X. Return carry clear if we
; found one, return a carry if no free lfns are left.

.proc   freefd

        ldx     #0
        clc
loop:   lda     fdtab,x
        beq     found
        inx
        cpx     #MAX_FDS
        bcc     loop
found:  rts

.endproc

;--------------------------------------------------------------------------
; Data

.bss
fdtab:  .res    MAX_FDS
unittab:.res    MAX_FDS



