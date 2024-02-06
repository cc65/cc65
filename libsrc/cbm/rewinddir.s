;
; Ullrich von Bassewitz, 2012-06-03
;
; Based on C code by Groepaz
;
; void __fastcall__ rewinddir (DIR *dir);
;


        .include        "dir.inc"
        .include        "zeropage.inc"

        .import         pushax

.proc   _rewinddir

        jsr     pushax          ; Push dir
        ldx     #0
        stx     sreg+1
        stx     sreg
        lda     #32             ; Pass 32 as offset ...
        jmp     _seekdir        ; ... to seekdir

.endproc

