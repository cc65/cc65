;
; Default __argv_mem: returns NULL so argc/argv are silently skipped.
; Override by providing storage for argv, e.g.
; void *__fastcall__ __argv_mem(size_t size) { return malloc(size); }
;

.export ___argv_mem

.proc ___argv_mem

        lda     #0
        tax
        rts

.endproc
