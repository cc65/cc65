;
; Christian Groessler, Oct-2000
; Daniel Serpell, Dec-2009
;
; the fdtable itself is defined here
;

        .include "fd.inc"

        .export fd_table,fd_index
        .export ___fd_table,___fd_index ; for test(debug purposes only

        .data

___fd_index:
fd_index:       ; fd number is index into this table, entry's value specifies the fd_table entry
        .byte   0,0,0           ; at start, three first files are stdin/stdout/stderr.
        .res    MAX_FD_INDEX-3,$ff

___fd_table:
fd_table:       ; each entry represents an open iocb
        .byte   3,0,'E',0       ; system console, app starts with opened iocb #0 for E:
        .byte   0,$ff,0,0
        .byte   0,$ff,0,0
        .byte   0,$ff,0,0
        .byte   0,$ff,0,0
        .byte   0,$ff,0,0
        .byte   0,$ff,0,0
        .byte   0,$ff,0,0

