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
        .word   __sig_dfl       ; SIGABRT
        .word   __sig_dfl       ; SIGFPE
        .word   __sig_dfl       ; SIGILL
        .word   __sig_dfl       ; SIGINT
        .word   __sig_dfl       ; SIGSEGV
        .word   __sig_dfl       ; SIGTERM


