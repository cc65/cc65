        .export         _open
		.import 		addysp,popax
		.importzp sp,tmp2,tmp3,tmp1
		; int open (const char* name, int flags, ...);    /* May take a mode argument */
    .include        "telemon30.inc"
		
.proc _open
; Throw away any additional parameters passed through the ellipsis

        dey                     ; Parm count < 4 shouldn't be needed to be...
        dey                     ; ...checked (it generates a c compiler warning)
        dey
        dey
        beq     parmok          ; Branch if parameter count ok
        jsr     addysp          ; Fix stack, throw away unused parameters

; Parameters ok. Pop the flags and save them into tmp3

parmok: jsr     popax           ; Get flags

; Get the filename from stack and parse it. Bail out if is not ok

        jsr     popax           ; Get name

		
		BRK_TELEMON XOPEN
		
       ; jsr     fnparse         ; Parse it
        ;tax
        ;bne     oserror         ; Bail out if problem with name

; Get a free file handle and remember it in tmp2

      ;  jsr     freefd
        ;lda     #EMFILE         ; Load error code
        ;bcs     seterrno        ; Jump in case of errors
        ;stx     tmp2
;
		
		
		rts
.endproc
		
		