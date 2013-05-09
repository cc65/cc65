;
; Ullrich von Bassewitz, 2002-12-26
;
; CC65 runtime: Jump vector that resides in the data segment so it's address 
; may be patched at runtime.
;

        .export jmpvec

.data

jmpvec: jmp     $FFFF


