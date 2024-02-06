;
; Ullrich von Bassewitz, 2003-12-28, 2009-09-26
;
; Common functions of the mouse driver API.
;

        .import         return0, popsreg, incsp2, mouse_libref
        .importzp       sreg, ptr1, tmp1, tmp2
        .interruptor    mouse_irq               ; Export as IRQ handler

        .include        "mouse-kernel.inc"



;----------------------------------------------------------------------------
; Variables


.bss
_mouse_drv:     .res    2               ; Pointer to driver

_mouse_hidden:  .res    1               ; Mouse visibility flag

; Jump table for the driver functions.
.data
mouse_vectors:
mouse_install:  jmp     return0
mouse_uninstall:jmp     return0
mouse_hide:     jmp     return0
mouse_show:     jmp     return0
mouse_setbox:   jmp     return0
mouse_getbox:   jmp     return0
mouse_move:     jmp     return0
mouse_buttons:  jmp     return0
mouse_pos:      jmp     return0
mouse_info:     jmp     return0
mouse_ioctl:    jmp     return0
mouse_irq:      .byte   $60, $00, $00   ; RTS plus two dummy bytes
mouse_flags:    .byte   $00

; Driver header signature
.rodata
mouse_sig:      .byte   $6d, $6f, $75, MOUSE_API_VERSION    ; "mou", version


.code
;----------------------------------------------------------------------------
; unsigned char __fastcall__ mouse_install (const struct mouse_callbacks* c,
;                                           void* driver);
; /* Install an already loaded driver. Returns an error code. */

_mouse_install:
        sta     _mouse_drv
        sta     ptr1
        stx     _mouse_drv+1
        stx     ptr1+1

; Check the driver signature

        ldy     #.sizeof(mouse_sig)-1
@L0:    lda     (ptr1),y
        cmp     mouse_sig,y
        bne     inv_drv
        dey
        bpl     @L0

; Set the library reference

        ldy     #MOUSE_HDR::LIBREF
        lda     #<mouse_libref
        sta     (ptr1),y
        iny
        lda     #>mouse_libref
        sta     (ptr1),y

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

; Copy the flags byte. It is located directly behind the jump vectors, so Y
; is already correct when we come here. To save code, we use copyjv - crude
; but effective.

        jsr     copyjv

; Copy the callback vectors into the driver space

        jsr     popsreg
        ldy     #(MOUSE_HDR::CALLBACKS + .sizeof(MOUSE_HDR::CALLBACKS) - 1)
        sty     tmp2
        ldy     #.sizeof(MOUSE_CALLBACKS)-1
        sty     tmp1

@L2:    jsr     copycb
        ldy     tmp1
        jsr     copycb
        dec     tmp2                    ; Skip opcode byte
        ldy     tmp1
        bpl     @L2

; Install the IRQ vector if the driver needs it

        bit     mouse_flags             ; Test MOUSE_FLAG_EARLY_IRQ
        bvc     @L3                     ; Jump if no interrupts at this time
        jsr     install_irq             ; Activate IRQ routine

; Call driver install routine and check for errors

@L3:    jsr     mouse_install
        tay                             ; Test error code
        bne     uninstall_irq           ; Jump on error

; No errors on INSTALL. If the driver needs late IRQs, enable them now. Be
; careful not to use A/X since these registers contain the error code from
; INSTALL.

        bit     mouse_flags             ; Test MOUSE_FLAG_LATE_IRQ
        bpl     Exit                    ; Jump if vector not needed
install_irq:
        ldy     #$4C                    ; Jump opcode
        sty     mouse_irq               ; Activate IRQ routine
Exit:   rts

; Uninstall IRQ vector if install routine had errors. A/X may contain the
; error code from mouse_install, so don't use it.

uninstall_irq:
        ldy     #$60                    ; RTS opcode
        sty     mouse_irq               ; Disable IRQ entry point
        rts

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
; unsigned char mouse_uninstall (void);
; /* Uninstall the currently loaded driver. Returns an error code. */

_mouse_uninstall:

; Depending on the late/early IRQ flag, we will disable IRQs before or after
; calling the driver mouse_uninstall routine.

        bit     mouse_flags             ; Test MOUSE_FLAG_LATE_IRQ
        bpl     @L1                     ; Don't disable interrupts now
        jsr     uninstall_irq           ; Disable driver interrupts
@L1:    jsr     mouse_uninstall         ; Call driver routine

; We don't check the flag a second time here, since disabling IRQs twice,
; or disabling them if they weren't enabled will do no harm, and the missing
; check will save a few bytes.

        jsr     uninstall_irq           ; Disable driver interrupts

_mouse_clear_ptr:                       ; External entry point
        lda     #0
        sta     _mouse_drv
        sta     _mouse_drv+1            ; Clear the driver pointer

        tax
        rts                             ; Return zero
