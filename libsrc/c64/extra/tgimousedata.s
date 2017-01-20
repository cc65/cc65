; C64 sprite addresses for the TGI mouse pointer
;
; 2017-01-13, Greg King

; In order to provide a visible mouse pointer during TGI's graphics mode,
; the object file "c64-tgimousedata.o" must be linked explicitly into
; a program file.  Example:
;
; cl65 -t c64 -o program-file main-code.c subroutines.s c64-tgimousedata.o
;
; Note:  Currently, a program cannot have default
; pointers for both text and graphic modes.

; The TGI graphics mode uses VIC-II's 16K bank number three.
;
; Address of the TGI bitmap's color RAM

COLORMAP        :=      $D000

        .export         mcb_spritepointer := COLORMAP + $03F8
        .export         mcb_spritememory  := COLORMAP + $0400
