;
; Ullrich von Bassewitz, 21.06.2002
;
; Common functions of the tgi graphics kernel.
;

        .import         tgi_libref
        .importzp       ptr1
        .interruptor    tgi_irq         ; Export as IRQ handler

        .include        "tgi-kernel.inc"
        .include        "tgi-error.inc"


;----------------------------------------------------------------------------
; Variables

.bss

_tgi_drv:           .res    2           ; Pointer to driver
; From here on, variables get cleared when a new driver is loaded
cstart:
_tgi_error:         .res    1           ; Last error code
_tgi_gmode:         .res    1           ; Flag: Graphics mode active
_tgi_curx:          .res    2           ; Current drawing cursor X
_tgi_cury:          .res    2           ; Current drawing cursor Y
_tgi_color:         .res    1           ; Current drawing color
_tgi_font:          .res    1           ; Which font to use
_tgi_textdir:       .res    1           ; Current text direction
_tgi_vectorfont:    .res    2           ; Pointer to vector font
; The following are character scale/size variables in 8.8 fixed point
; format. They are required to be in this order and adjacent.
_tgi_textscalew:    .res    2           ; Vector font width scale
                    .res    2           ; Bitmap font width scale
_tgi_charwidth:     .res    2           ; Full width of one bitmap char
_tgi_textscaleh:    .res    2           ; Vector font height scale
                    .res    2           ; Bitmap font height scale
_tgi_charheight:    .res    2           ; Full height of one bitmap char

; End of section that gets cleared when a new driver is loaded
csize   = * - cstart

; Maximum X and Y coordinate (that is, xres-1 and yres-1)
_tgi_xmax:          .res    2
_tgi_ymax:          .res    2

; The following variables are copied from the driver header for faster access.
; fontwidth and fontheight are expected to be in order and adjacent.
tgi_driver_vars:
_tgi_xres:          .res    2           ; X resolution of the current mode
_tgi_yres:          .res    2           ; Y resolution of the current mode
_tgi_colorcount:    .res    1           ; Number of available colors
_tgi_pagecount:     .res    1           ; Number of available screen pages
_tgi_fontwidth:     .res    1           ; System font width in pixels
_tgi_fontheight:    .res    1           ; System font height in pixels
_tgi_aspectratio:   .res    2           ; Aspect ratio in 8.8 fixed point
_tgi_flags:         .res    1           ; TGI driver flags


.data

; Jump table for the driver functions.

jumpvectors:
tgi_install:        jmp     $0000
tgi_uninstall:      jmp     $0000
tgi_init:           jmp     $0000
tgi_done:           jmp     $0000
tgi_geterror:       jmp     $0000
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
tgi_line:           jmp     $0000
tgi_bar:            jmp     $0000
tgi_textstyle:      jmp     $0000
tgi_outtext:        jmp     $0000
tgi_irq:            .byte   $60, $00, $00       ; RTS plus two dummy bytes

; Driver header signature
.rodata
tgi_sig:        .byte   $74, $67, $69, TGI_API_VERSION  ; "tgi", version


.code
;----------------------------------------------------------------------------
; void __fastcall__ tgi_install (void* driver);
; /* Install an already loaded driver. */


_tgi_install:
        sta     _tgi_drv
        sta     ptr1
        stx     _tgi_drv+1
        stx     ptr1+1

; Check the driver signature

        ldy     #.sizeof(tgi_sig)-1
@L0:    lda     (ptr1),y
        cmp     tgi_sig,y
        bne     tgi_inv_drv
        dey
        bpl     @L0

; Set the library reference

        ldy     #TGI_HDR::LIBREF
        lda     #<tgi_libref
        sta     (ptr1),y
        iny
        lda     #>tgi_libref
        sta     (ptr1),y

; Copy the jump vectors

        ldy     #TGI_HDR::JUMPTAB
        ldx     #0
@L1:    inx                             ; Skip JMP opcode
        jsr     copy                    ; Copy one byte
        jsr     copy                    ; Copy one byte
        cpy     #(TGI_HDR::JUMPTAB + .sizeof(TGI_HDR::JUMPTAB))
        bne     @L1

; Call the driver install routine. It may update header variables, so we copy
; them after this call.

        jsr     tgi_install

; Copy variables from the driver header for faster access.

        jsr     tgi_set_ptr             ; Set ptr1 to tgi_drv
        ldy     #(TGI_HDR::VARS + .sizeof(TGI_HDR::VARS) - 1)
        ldx     #.sizeof(TGI_HDR::VARS)-1
@L3:    lda     (ptr1),y
        sta     tgi_driver_vars,x
        dey
        dex
        bpl     @L3

; Install the IRQ vector if the driver needs it.

        lda     tgi_irq+2               ; Check high byte of IRQ vector
        beq     @L4                     ; Jump if vector invalid
        lda     #$4C                    ; Jump opcode
        sta     tgi_irq                 ; Activate IRQ routine

; Initialize some other variables

        lda     #$00
@L4:    ldx     #csize-1
@L5:    sta     cstart,x                ; Clear error/mode/curx/cury/...
        dex
        bpl     @L5

        rts

; Copy one byte to the jump vectors

copy:   lda     (ptr1),y
        sta     jumpvectors,x
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
        lda     _tgi_drv
        sta     ptr1
        lda     _tgi_drv+1
        sta     ptr1+1
        rts

;----------------------------------------------------------------------------
; void tgi_uninstall (void);
; /* Uninstall the currently loaded driver but do not unload it. Will call
; ** tgi_done if necessary.
; */

_tgi_uninstall:
        jsr     _tgi_done               ; Switch off graphics

        jsr     tgi_uninstall           ; Allow the driver to clean up

        lda     #$60                    ; RTS opcode
        sta     tgi_irq                 ; Disable IRQ entry point

; Clear driver pointer and error code

tgi_clear_ptr:
        lda     #$00
        sta     _tgi_drv
        sta     _tgi_drv+1
        sta     _tgi_error

        rts
