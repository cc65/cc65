;
; int fastcall vcscanf(const char* format, va_list ap);
;
; 2014-09-10, Greg King
;

        .export         _vcscanf

        .import         _cgetc, _cputc
        .import         popax, pushax, swapstk

        .include        "../common/_scanf.inc"


; static bool pushed;
; static char back;
;
        .bss
pushed: .res    1
back:   .res    1

        .code
; /* Call-back functions:
; ** (Note:  These prototypes must NOT be declared with fastcall!  They don't
; ** use (getfunc)'s and (ungetfunc)'s last parameter.  Leaving it out of these
; ** prototypes makes more efficient code.)
; */

; ----------------------------------------------------------------------------
; /* Read a character from the console, and return it to an internal function */
; static int get(void) {
;     static char C;
;
;     if (pushed) {
;         pushed = false;
;         return (int)back;
;         }
;     cputc(C = cgetc());       /* echo a typed character */
;     return (int)C;
;     }
;
get:    ldx     pushed
        beq     L1

; Return the old, pushed-back character (instead of getting a new one).
;
        dex                     ; ldx #>$0000
        stx     pushed
        lda     back
        rts

; Directly read the keyboard.
;
L1:     jsr     _cgetc

; Echo the character to the screen.
;
        pha
        jsr     _cputc
        pla
        ldx     #>$0000
        rts


; ----------------------------------------------------------------------------
; static int cdecl unget(int c) {
;     pushed = true;
;     return back = c;
;     }
;
unget:  ldx     #1
        stx     pushed
        jsr     popax           ; get the first argument
        sta     back
        rts


; ----------------------------------------------------------------------------
; int fastcall vcscanf(const char* format, va_list ap) {
;     /* Initiate the data structure.
;     ** Don't initiate the member that these conio functions don't use.
;     */
;     static const struct scanfdata d = {
;         (  getfunc)  get,
;         (ungetfunc)unget
;         };
;
;     /* conio is very interactive.  So, don't use any pushed-back character.
;     ** Start fresh, each time that this function is called.
;     */
;     pushed = false;
;
;     /* Call the internal function, and return the result. */
;     return _scanf(&d, format, ap);
;     }
;
; Beware:  Because ap is a fastcall parameter, we must not destroy .XA.
;
        .proc   _vcscanf

; ----------------------------------------------------------------------------
; Static, constant scanfdata structure for the _vcscanf routine.
;
        .rodata
d:      .addr   get             ; SCANFDATA::GET
        .addr   unget           ; SCANFDATA::UNGET
;       .addr   0               ; SCANFDATA::DATA (not used)

        .code
        pha                     ; Save low byte of ap
        txa
        pha                     ; Save high byte of ap
        ldx     #0
        stx     pushed

; Put &d on the stack in front of the format pointer.

        lda     #<d
        ldx     #>d
        jsr     swapstk         ; Swap .XA with top-of-stack
        jsr     pushax          ; Put format pointer back on stack

; Restore ap, and jump to _scanf which will clean up the stack.

        pla
        tax
        pla
        jmp     __scanf
        .endproc
