;
; Ullrich von Bassewitz, 21.06.2002
;
; Common functions of the tgi graphics kernel.
;

        .include        "tgi-kernel.inc"

        .importzp       ptr1


;----------------------------------------------------------------------------
; Variables

.bss

_tgi_drv:	.res	2		; Pointer to driver
_tgi_error:	.res	1		; Last error code
_tgi_mode:      .res    1               ; Graphics mode or zero


.data
                         
; Jump table for the driver functions.

tgi_install:	jmp	$0000
tgi_deinstall:	jmp	$0000
tgi_init:       jmp	$0000
tgi_post:       jmp	$0000
tgi_control:    jmp	$0000
tgi_clear:      jmp     $0000
tgi_setcolor:   jmp     $0000
tgi_setpixel:   jmp     $0000
tgi_getpixel:   jmp     $0000
tgi_line:       jmp     $0000
tgi_bar:        jmp     $0000
tgi_circle:     jmp     $0000


;----------------------------------------------------------------------------
; Setup the TGI driver once it is loaded.

tgi_setup:
	lda	_tgi_drv
	sta	ptr1
	lda	_tgi_drv+1
	sta	ptr1+1

        ldy     #TGI_HDR_JUMPTAB
        ldx     #1

@L1:    lda     (ptr1),y
        sta     tgi_install,x
        iny
        inx
        lda     (ptr1),y
        sta     tgi_install,x
        inx
        cpx     #(TGI_HDR_JUMPCOUNT*3)
        bne     @L1
                
; Initialize variables

        lda     #$00
        sta     _tgi_error
        sta     _tgi_mode

        jsr     tgi_install             ; Call driver install routine

;       jmp     tgi_fetch_error

;----------------------------------------------------------------------------
; Fetch the error code from the driver and place it into the global error
; variable.

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

