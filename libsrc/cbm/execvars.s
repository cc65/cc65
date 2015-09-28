;
; Platform-specific variables for the exec program-chaining function
;

.if     .defined (__C128__)
        .include        "c128.inc"
.elseif .defined (__C16__)
        .include        "c16.inc"
.elseif .defined (__C64__)
        .include        "c64.inc"
.elseif .defined (__CBM510__)
        .include        "cbm510.inc"
.elseif .defined (__CBM610__)
        .include        "cbm610.inc"
.elseif .defined (__PET__)
        .include        "pet.inc"
.elseif .defined (__PLUS4__)
        .include        "plus4.inc"
.else
        .include        "vic20.inc"
.endif

; exec() is written in C.
; Provide the spellings that the C compiler wants to use.

.ifdef  VARTAB
.exportzp _vartab       :=      VARTAB
.exportzp _memsize      :=      MEMSIZE
.endif

.exportzp _txtptr       :=      TXTPTR

.export   _basbuf       :=      BASIC_BUF
.exportzp _basbuf_len   =       BASIC_BUF_LEN
