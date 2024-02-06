;
; Ullrich von Bassewitz, 16.11.2002
;
; File descriptor management for the POSIX I/O routines
;

.global         fdtab   ; Handle table
.global         unittab ; Unit table
.global         freefd  ; Return a table entry

MAX_FDS         = 8     ; Maximum number of file descriptors
LFN_OFFS        = 3     ; Start with logical file number 3

LFN_CLOSED      = $00   ; LFN is closed
LFN_READ        = $01   ; Open for reading
LFN_WRITE       = $02   ; Open for writing
LFN_OPEN        = (LFN_READ | LFN_WRITE)
LFN_EOF         = $80   ; Read to EOF

LFN_STDIN       = LFN_OFFS+0
LFN_STDOUT      = LFN_OFFS+1
LFN_STDERR      = LFN_OFFS+2

