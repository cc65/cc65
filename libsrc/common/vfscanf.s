;
; int __fastcall__ vfscanf (FILE* f, const char* format, va_list ap);
;
; Ullrich von Bassewitz, 2004-11-27
;

  	.export	      	_vfscanf
        .import         _fgetc, _ungetc

        .include        "zeropage.inc"
        .include        "_scanf.inc"


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
;     struct scanfdata d;
;
;     /* Initialize the data struct. We do only need the given file as user data,
;      * since the get and ungetc are crafted so they match the standard fgetc
;      * and ungetc functions.
;      */
;     d.get    = (getfunc) fgetc,
;     d.unget  = (ungetfunc) ungetc,
;     d.data   = f;
;
;     /* Call the internal function and return the result */
;     return _scanf (&d, format, ap);
; }
;
; Since _scanf has the same parameter stack as vfscanf, with f replaced by &d,
; we will do exactly that. _scanf will then clean up the stack, so we can jump
; directly there, no need to return.
; Beware: Since ap is a fastcall parameter, we must not destroy a/x.
;

.code
_vfscanf:
        pha                     ; Save low byte of ap

; Swap f against &d on the stack, placing f into d.data

        ldy     #2              ; Offset of f on the stack
        lda     (sp),y
        sta     d + SCANFDATA::DATA
        lda     #<d
        sta     (sp),y

        iny                     ; High byte
        lda     (sp),y
        sta     d + SCANFDATA::DATA + 1
        lda     #>d
        sta     (sp),y

; Restore the low byte of ap and jump to the _scanf function

        pla
        jmp     __scanf

