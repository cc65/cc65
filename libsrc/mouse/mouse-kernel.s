;
; Ullrich von Bassewitz, 2003-12-28
;
; Common functions of the mouse driver API.
;

        .import         return0, popsreg, incsp2
        .importzp       sreg, ptr1, tmp1, tmp2
       	.condes	       	mouse_irq, 2		; Export as IRQ handler

        .include        "mouse-kernel.inc"



;----------------------------------------------------------------------------
; Variables


.bss
_mouse_drv:     .res    2      		; Pointer to driver

_mouse_hidden:  .res    1               ; Mouse visibility flag

; Jump table for the driver functions.
.data
mouse_vectors:
mouse_install:  jmp     return0
mouse_uninstall:jmp     return0
mouse_hide:     jmp     return0
mouse_show:     jmp     return0
mouse_box:      jmp     return0
mouse_move:     jmp     return0
mouse_buttons:  jmp     return0
mouse_pos:      jmp     return0
mouse_info:     jmp     return0
mouse_ioctl:    jmp     return0
mouse_irq:	.byte	$60, $00, $00	; RTS plus two dummy bytes

; Driver header signature
.rodata
mouse_sig:      .byte   $6d, $6f, $75, MOUSE_API_VERSION    ; "mou", version


;----------------------------------------------------------------------------
; unsigned char __fastcall__ mouse_install (const struct mouse_callbacks* c,
;                                           void* driver);
; /* Install an already loaded driver. Returns an error code. */

_mouse_install:
       	sta     _mouse_drv
  	sta	ptr1
   	stx     _mouse_drv+1
  	stx    	ptr1+1

; Check the driver signature

        ldy     #.sizeof(mouse_sig)-1
@L0:    lda     (ptr1),y
        cmp     mouse_sig,y
        bne     inv_drv
        dey
        bpl     @L0

; Reset flags

        lda     #1
        sta     _mouse_hidden

; Copy the jump vectors

        ldy     #MOUSE_HDR::JUMPTAB
        ldx     #0
@L1:    inx                             ; Skip the JMP opcode
        jsr     copyjv                  ; Copy one byte
        jsr     copyjv                  ; Copy one byte
        cpy     #(MOUSE_HDR::JUMPTAB + .sizeof(MOUSE_HDR::JUMPTAB))
        bne     @L1

; Copy the callback vectors into the driver space

        jsr     popsreg
        ldy     #(MOUSE_HDR::CALLBACKS + .sizeof(MOUSE_HDR::CALLBACKS) - 1)
        sty     tmp2
        ldy     #.sizeof(MOUSE_CALLBACKS)-1
        sty     tmp1

@L2:    jsr     copycb
        jsr     copycb
        dec     tmp2                    ; Skip opcode byte
        ldy     tmp1
        bpl     @L2

; Call driver install routine

        jsr     mouse_install

; Install the IRQ vector if the driver needs it. A/X contains the error code
; from mouse_install, so don't use it.

        ldy     mouse_irq+2             ; Check high byte of IRQ vector
        beq     @L3                     ; Jump if vector invalid
   	ldy	#$4C			; Jump opcode
       	sty    	mouse_irq               ; Activate IRQ routine
@L3:    rts

; Driver signature invalid. One word is still on the stack

inv_drv:
        lda     #MOUSE_ERR_INV_DRIVER
        ldx     #0
        jmp     incsp2

; Copy one byte from the jump vectors

copyjv: lda     (ptr1),y
        sta     mouse_vectors,x
        iny
        inx
        rts

; Copy one byte from the callback vectors

copycb: lda     (sreg),y
        dec     tmp1
        ldy     tmp2
        sta     (ptr1),y
        dec     tmp2
        rts

;----------------------------------------------------------------------------
; unsigned char __fastcall__ mouse_uninstall (void);
; /* Uninstall the currently loaded driver. Returns an error code. */

_mouse_uninstall:
        jsr     mouse_uninstall         ; Call driver routine

	lda	#$60                    ; RTS opcode
	sta	mouse_irq               ; Disable IRQ entry point

mouse_clear_ptr:                        ; External entry point
        lda     #0
        sta     _mouse_drv
        sta     _mouse_drv+1            ; Clear the driver pointer

        tax
        rts                             ; Return zero

