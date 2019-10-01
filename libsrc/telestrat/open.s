    .export    _open

    .import    addysp,popax

    .importzp  sp,tmp2,tmp3,tmp1


    .include   "telestrat.inc"
    .include   "errno.inc"
    .include   "fcntl.inc"

; int open (const char* name, int flags, ...);    /* May take a mode argument */
.proc _open
; Throw away any additional parameters passed through the ellipsis

    dey                     ; Parm count < 4 shouldn't be needed to be...
    dey                     ; ...checked (it generates a c compiler warning)
    dey
    dey
    beq         parmok      ; Branch if parameter count ok
    jsr         addysp      ; Fix stack, throw away unused parameters

; Parameters ok. Pop the flags and save them into tmp3

parmok:
    jsr         popax       ; Get flagss
    sta         tmp3        ; save flags
; Get the filename from stack and parse it. Bail out if is not ok
    jsr         popax       ; Get name
    ldy         tmp3        ; Get flags again
    BRK_TELEMON XOPEN       ; launch primitive ROM
    rts
.endproc
