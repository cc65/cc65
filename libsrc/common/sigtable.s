;
; Ullrich von Bassewitz, 2002-12-16
;
; Signal vector table
;

        .export         sigtable

        .include        "signal.inc"

;----------------------------------------------------------------------------
;

.data

sigtable:
        .word   ___sig_dfl      ; SIGABRT
        .word   ___sig_dfl      ; SIGFPE
        .word   ___sig_dfl      ; SIGILL
        .word   ___sig_dfl      ; SIGINT
        .word   ___sig_dfl      ; SIGSEGV
        .word   ___sig_dfl      ; SIGTERM


