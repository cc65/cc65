;
; Ullrich von Bassewitz, 21.06.2002
;
; Common functions of the tgi graphics kernel.
;

        .include        "tgi-kernel.inc"
        .include        "tgi-error.inc"

        .export         tgi_clear_ptr
        .importzp       ptr1


;----------------------------------------------------------------------------
; Variables

.bss

_tgi_drv:      	    .res    2		; Pointer to driver
_tgi_error:    	    .res    1		; Last error code
_tgi_gmode:         .res    1           ; Flag: Graphics mode active
_tgi_curx:          .res    2           ; Current drawing cursor X
_tgi_cury:          .res    2           ; Current drawing cursor Y
_tgi_color:         .res    1           ; Current drawing color
_tgi_textdir:       .res    1           ; Current text direction
_tgi_textmagx:      .res    1           ; Text magnification in X dir
_tgi_textmagy:      .res    1           ; Text magnification in Y dir

; The following variables are copied from the driver header for faster access
tgi_driver_vars:
_tgi_xres:          .res    2           ; X resolution of the current mode
_tgi_yres:          .res    2           ; Y resolution of the current mode
_tgi_colorcount:    .res    1           ; Number of available colors
_tgi_pagecount:     .res    1           ; Number of available screen pages
_tgi_fontsizex:     .res    1           ; System font X size
_tgi_fontsizey:     .res    1           ; System font Y size
tgi_driver_var_size     = * - tgi_driver_vars


.data

; Jump table for the driver functions.

tgi_install:   	    jmp     $0000
tgi_uninstall: 	    jmp     $0000
tgi_init:           jmp     $0000
tgi_done:           jmp     $0000
tgi_geterror:	    jmp	    $0000
tgi_control:        jmp     $0000
tgi_clear:          jmp     $0000
tgi_setviewpage:    jmp     $0000
tgi_setdrawpage:    jmp     $0000
tgi_setcolor:       jmp     $0000
tgi_setpalette:     jmp     $0000
tgi_getpalette:     jmp     $0000
tgi_getdefpalette:  jmp     $0000
tgi_setpixel:       jmp     $0000
tgi_getpixel:       jmp     $0000
tgi_horline:        jmp     $0000
tgi_line:           jmp     $0000
tgi_bar:            jmp     $0000
tgi_circle:         jmp     $0000
tgi_textstyle:      jmp     $0000
tgi_outtext:        jmp     $0000

; Driver header signature
.rodata
tgi_sig:        .byte   $74, $67, $69, $00      ; "tgi", version
tgi_sig_len     = * - tgi_sig


;----------------------------------------------------------------------------
; void __fastcall__ tgi_install (void* driver);
; /* Install an already loaded driver. */


_tgi_install:
       	sta     _tgi_drv
  	sta 	ptr1
  	stx     _tgi_drv+1
  	stx    	ptr1+1

; Check the driver signature

        ldy     #tgi_sig_len-1
@L0:    lda     (ptr1),y
        cmp     tgi_sig,y
        bne     tgi_inv_drv
        dey
        bpl     @L0

; Copy the jump vectors

        ldy     #TGI_HDR_JUMPTAB
        ldx     #0
@L1:    inx                             ; Skip JMP opcode
        jsr     copy                    ; Copy one byte
        jsr     copy                    ; Copy one byte
        cpx     #(TGI_HDR_JUMPCOUNT*3)
        bne     @L1

; Check for emulation vectors needed

        lda     tgi_bar+1
        ora     tgi_bar+2               ; Do we have a BAR vector?
        bne     @L2                     ; Jump if yes
        lda     #<tgi_emu_bar           ; Use emulation if no
        sta     tgi_bar+1
        lda     #>tgi_emu_bar
        sta     tgi_bar+2

@L2:    jsr     tgi_install             ; Call driver install routine, may...
                                        ; ...update variables
        jsr     tgi_set_ptr             ; Set ptr1 to tgi_drv

; Copy variables. Beware: We are using internal knowledge about variable
; layout here!

        ldy     #TGI_HDR_XRES
        ldx     #0
@L3:    lda     (ptr1),y
        sta     tgi_driver_vars,x
        iny
        inx
        cpx     #tgi_driver_var_size
        bne     @L3

; Initialize variables

        lda     #$00
        ldx     #7-1
@L4:    sta     _tgi_error,x            ; Clear error/mode/curx/cury/textdir
        dex
        bpl     @L4

	rts

; Copy one byte from the jump vectors

copy:   lda     (ptr1),y
        sta     tgi_install,x
        iny
        inx
        rts

;----------------------------------------------------------------------------
; Set an invalid argument error

tgi_inv_arg:
        lda     #TGI_ERR_INV_ARG
        sta     _tgi_error
        rts

;----------------------------------------------------------------------------
; Set an invalid driver error

tgi_inv_drv:
        lda     #TGI_ERR_INV_DRIVER
        sta     _tgi_error
        rts

;----------------------------------------------------------------------------
; Load the pointer to the tgi driver into ptr1.

tgi_set_ptr:
       	lda 	_tgi_drv
  	sta 	ptr1
  	lda 	_tgi_drv+1
  	sta 	ptr1+1
        rts

;----------------------------------------------------------------------------
; void __fastcall__ tgi_uninstall (void);
; /* Uninstall the currently loaded driver but do not unload it. Will call
;  * tgi_done if necessary.
;  */

_tgi_uninstall:
        jsr     _tgi_done               ; Switch off graphics
        jsr     tgi_uninstall           ; Allow the driver to clean up

; Clear driver pointer and error code

tgi_clear_ptr:                          ; External entry point
        lda     #$00
        sta     _tgi_drv
        sta     _tgi_drv+1
        sta     _tgi_error

        rts


