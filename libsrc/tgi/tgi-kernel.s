;
; Ullrich von Bassewitz, 21.06.2002
;
; Common functions of the tgi graphics kernel.
;

        .include        "tgi-kernel.inc"

        .export         _tgi_setup
        .importzp       ptr1


;----------------------------------------------------------------------------
; Variables

.bss

_tgi_drv:	.res	2		; Pointer to driver
_tgi_error:	.res	1		; Last error code
_tgi_mode:      .res    1               ; Graphics mode or zero
_tgi_xres:      .res    2               ; X resolution of the current mode
_tgi_yres:      .res    2               ; Y resolution of the current mode
        

.data

; Jump table for the driver functions.

tgi_install:	jmp	$0000
tgi_deinstall:	jmp	$0000
tgi_init:       jmp	$0000
tgi_done:       jmp	$0000
tgi_control:    jmp	$0000
tgi_clear:      jmp     $0000
tgi_setcolor:   jmp     $0000
tgi_setpixel:   jmp     $0000
tgi_getpixel:   jmp     $0000
tgi_line:       jmp     $0000
tgi_bar:        jmp     $0000
tgi_circle:     jmp     $0000


;----------------------------------------------------------------------------
; void __fastcall__ tgi_setup (void);
; /* Setup the driver and graphics kernel once the driver is loaded */


copy:   lda     (ptr1),y
        sta     tgi_install,x
        iny
        inx
        rts


_tgi_setup:
 	jsr     tgi_set_ptr             ; load _tgi_drv into ptr1

; Copy the jump vectors

        ldy     #TGI_HDR_JUMPTAB
        ldx     #0
@L1:    inx                             ; Skip JMP opcode
        jsr     copy                    ; Copy one byte
        jsr     copy                    ; Copy one byte
        cpx     #(TGI_HDR_JUMPCOUNT*3)
        bne     @L1

; Copy the screen dimensions

        ldy     #TGI_HDR_XRES
        lda     (ptr1),y
        sta     _tgi_xres
        iny
        lda     (ptr1),y
        sta     _tgi_xres+1
        ldy     #TGI_HDR_YRES
        lda     (ptr1),y
        sta     _tgi_yres
        iny
        lda     (ptr1),y
        sta     _tgi_yres+1

; Initialize variables

        lda     #$00
        sta     _tgi_error
        sta     _tgi_mode

        jsr     tgi_install             ; Call driver install routine

;       jmp     tgi_fetch_error

;----------------------------------------------------------------------------
; Fetch the error code from the driver and place it into the global error
; variable. The function will also return the error in A and the flags from
; loading the error code are set.

tgi_fetch_error:
        jsr     tgi_set_ptr
  	ldy	#TGI_HDR_ERROR
  	lda	(ptr1),y
  	sta	_tgi_error
  	rts

;----------------------------------------------------------------------------
; Load the pointer to the tgi driver into ptr1.

tgi_set_ptr:
  	lda	_tgi_drv
  	sta	ptr1
  	lda	_tgi_drv+1
  	sta	ptr1+1
        rts

