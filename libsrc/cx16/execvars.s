;
; Platform-specific variables for the exec program-chaining function
;

        .include        "cx16.inc"

; exec() is written in C.
; Provide the spellings that the C compiler wants to use.

.export _vartab         :=      VARTAB
.export _memsize        :=      MEMSIZE

.exportzp _txtptr       :=      TXTPTR

.export   _basbuf       :=      BASIC_BUF
.exportzp _basbuf_len   =       BASIC_BUF_LEN
