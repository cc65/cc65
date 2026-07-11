;
; Default argv_mem: returns NULL so argc/argv are silently skipped.
; Override by providing storage for argv, e.g.
; void *__fastcall__ argv_mem(size_t size) { return malloc(size); }
;

.export _argv_mem

.proc _argv_mem

        lda     #0
        tax
        rts

.endproc
