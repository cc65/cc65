;
; int __fastcall__ vfscanf (FILE* f, const char* format, va_list ap);
;
; 2004-11-27, Ullrich von Bassewitz
; 2004-12-21, Greg King
;

        .export         _vfscanf
        .import         _fgetc, _ungetc, _ferror

        .include        "zeropage.inc"
        .include        "_scanf.inc"
        .include        "stdio.inc"


count   :=      ptr3            ; Result of scan


; ----------------------------------------------------------------------------
; Static scanfdata structure for the _vfscanf routine
;

.data
d:      .addr   _fgetc          ; GET
        .addr   _ungetc         ; UNGET
        .addr   0               ; data


; ----------------------------------------------------------------------------
; int __fastcall__ vfscanf (FILE* f, const char* format, va_list ap)
; /* Standard C function */
; {
;     /* Initialize the data struct. We do only need the given file as user data,
;     ** because the (getfunc) and (ungetfunc) functions are crafted so that they
;     ** match the standard-I/O fgetc() and ungetc().
;     */
;     static struct scanfdata d = {
;         (  getfunc)  fgetc,
;         (ungetfunc) ungetc
;     };
;     static int count;
;
;     d.data = (void*) f;
;
;     /* Call the internal function */
;     count = _scanf (&d, format, ap);
;
;     /* And, return the result */
;     return ferror (f) ? EOF : count;
; }
;
; Because _scanf() has the same parameter stack as vfscanf(), with f replaced
; by &d, we will do exactly that.  _scanf() then will clean up the stack.
; Beware: Since ap is a fastcall parameter, we must not destroy a/x.
;

.code
_vfscanf:
        pha                     ; Save low byte of ap

; Swap f against &d on the stack, placing f into d.data

        ldy     #2              ; Offset of f on the stack
        lda     (c_sp),y
        sta     d + SCANFDATA::DATA
        lda     #<d
        sta     (c_sp),y

        iny                     ; High byte
        lda     (c_sp),y
        sta     d + SCANFDATA::DATA + 1
        lda     #>d
        sta     (c_sp),y

; Restore the low byte of ap, and call the _scanf function

        pla
        jsr     __scanf
        sta     count
        stx     count+1

; Return -1 if there was a read error during the scan

        lda     d + SCANFDATA::DATA     ; Get f
        ldx     d + SCANFDATA::DATA+1
        jsr     _ferror
        tay
        beq     L1
        lda     #<EOF
        tax
        rts

; Or, return the result of the scan

L1:     lda     count
        ldx     count+1
        rts

